#include "server.h"

Server::Server(std::string anAddress, int aPort) :
	address(anAddress),
	port(aPort)
{}

void Server::run()
{
	on_init();
	on_serve();
	on_cleanup();
}

void Server::on_init()
{
	int opt;

	opt = 1;
	main_socket = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(port));
	to_non_blocking(main_socket);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr(address.c_str());
	memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
	bind(main_socket, (struct sockaddr *) &server_address, sizeof(server_address));
	if (listen(main_socket, 5) == -1)
	{
		std::cerr << "Not able to listen" << std::endl;
		exit(-1);
	}
	std::signal(SIGINT, check_running);
	std::signal(SIGTERM, check_running);

	tv.tv_sec = 1;
	tv.tv_usec = 0;
	std::cout << "Server initialization successed" << std::endl;
}

void Server::on_serve()
{
	int retval;
	int	highest_sock;

	std::cout << "Listening for new connexions on " << address << " (" << port << ")"
		" socket number " << main_socket << std::endl;
	while (g_running)
	{
		init_fdsets();
		highest_sock = get_highest_sock_number();
		retval = select(highest_sock,
				&read_sockets,
				&write_sockets,
				NULL, &tv);
		if (retval > 0)
		{
			if (FD_ISSET(main_socket, &read_sockets))
				add_client();
			on_listen();
			on_process_message();
			update_clients();
			process_channel_message();
			on_write();
		}
	}
}

void Server::init_fdsets()
{
	FD_ZERO(&read_sockets);
	FD_SET(main_socket, &read_sockets);
	for (Client aclient : clients)
	{
		if (aclient.get_sock() > 0)
		{
			FD_SET(aclient.get_sock(), &read_sockets);
			FD_SET(aclient.get_sock(), &write_sockets);
		}
	}
}

int	Server::get_highest_sock_number()
{
	int temp = 0;

	for (Client a_client : clients)
		if (a_client.get_sock() > temp)
			temp = a_client.get_sock();
	if (temp < main_socket)
		temp = main_socket;
	return (temp + 1);
}


void Server::add_client()
{
	int	sock;

	sock = accept(main_socket, (struct sockaddr *) NULL, NULL);
	to_non_blocking(sock);
	Client new_client(sock, current_id);
	current_id++;
	clients.push_back(new_client);
}

void Server::on_listen()
{
	int retvalue;

	if (clients.size() > 0)
	{
		for (auto a_client = clients.begin(); a_client != clients.end(); ++a_client)
		{
			if (FD_ISSET(a_client->get_sock(), &read_sockets))
			{
				retvalue = a_client->onread();
				if (retvalue == FAILED)
					std::cerr << "Problem while reading socket " << std::endl;
			}
		}
	}
}

void Server::delete_client(std::vector<Client>::iterator aclient)
{
	std::string name;

	name = aclient->get_name();
	std::cout << "Deleting client : " << aclient->get_name().c_str() << std::endl;
	std::cout << "Socket " << aclient->get_sock() << std::endl;
	close(aclient->get_sock());
	aclient->set_sock(TODELETE);
	notify_all_client(name);
}

void Server::notify_all_client(std::string name)
{
	std::string content;

	content = name + " has leaved the server.";
	Message message("chat", content, "server");
	for (auto client = clients.begin(); client != clients.end(); ++client)
		client->writing_queue.push(message);
}

void Server::on_process_message()
{
	for (auto aclient = clients.begin(); aclient != clients.end(); ++aclient)
	{
		while (!aclient->reading_queue.empty())
		{
			Message amessage = aclient->reading_queue.front();
			switch (amessage.get_command())
			{
				case CHAT: process_chat(aclient);
						   break;
				case NAME: process_name(aclient);
						   break;
				case LIST: process_list(aclient);
						   break;
				case EOC: delete_client(aclient);
						  break;
				case JOIN_CHAN: process_joinchan(aclient);
						   break;
				case CREATE_CHAN: process_createchan(aclient);
						   break;
				case LEAVE_CHAN: process_leavechan(aclient);
						   break;
				case DELETE_CHAN: process_deletechan(amessage);
						   break;
				default:
						   break;
			}
			aclient->reading_queue.pop();
		}
	}
}

void Server::process_name(std::vector<Client>::iterator aclient)
{
	Message amessage;

	amessage = aclient->reading_queue.front();
	aclient->set_name(trim(amessage.get_content()));
	std::cout << "Naming " << aclient->get_name() << std::endl;
}

void Server::process_list(std::vector<Client>::iterator aclient)
{
	std::string content = "Users\n";
	int size;

	for (auto aclient = clients.begin(); aclient != clients.end(); ++aclient)
		if (aclient->get_sock() > 0)
			content = content + aclient->get_name() + " ";
	content += "\nChannels\n";
	for (auto channel : channels)
		content = content + channel.get_name() + " ";
	Message message("list", content);
	aclient->writing_queue.push(message);
}

void Server::process_chat(std::vector<Client>::iterator aclient)
{
	Message src_message;
	Message message_echo;

	src_message = aclient->reading_queue.front();
	if (is_message_to_channel(src_message.get_dst()))
	{
		for (auto channel = channels.begin(); channel != channels.end(); ++channel)
		{
			if (channel->get_name() == src_message.get_dst())
			{
				std::string dst = aclient->get_name() + "(" + channel->get_name() + ")";
				Message dst_message(src_message.get_command_verbose(),
						src_message.get_content(),
						dst.c_str());
				channel->writing_queue.push(dst_message);
			}
		}
	}
	else
	{
		message_echo = build_message_echo(src_message, aclient);
		aclient->writing_queue.push(message_echo);
		Message dst_message(src_message.get_command_verbose(),
				src_message.get_content(),
				aclient->get_name().c_str());
		for (auto dst_client = clients.begin(); dst_client != clients.end(); ++dst_client)
		{
			if (dst_client->get_name() == src_message.get_dst().c_str())
			{
				std::cout << "Adding message to " << dst_client->get_name() << std::endl;
				dst_client->writing_queue.push(dst_message);
				break;
			}
		}
	}
}

bool Server::is_message_to_channel(std::string dst)
{
	for (auto channel : channels)
		if (channel.get_name() == dst)
			return true;
	return false;
}

Message Server::build_message_echo(Message src, std::vector<Client>::iterator aclient)
{
	std::string src_name;

	src_name = aclient->get_name() + "(" + src.get_dst().c_str() + ")";
	Message retval("chat", src.get_content(), src_name.c_str());
	std::cout << retval.get_message() << std::endl;
	return (retval);
}

void Server::process_joinchan(std::vector<Client>::iterator aclient)
{
	Message amessage;

	amessage = aclient->reading_queue.front();
	for (auto achannel = channels.begin(); achannel != channels.end(); ++achannel)
	{
		if (achannel->get_name() == amessage.get_content_string())
			achannel->add_client(aclient->get_sock());
	}
}

void Server::process_leavechan(std::vector<Client>::iterator aclient)
{
	Message amessage;

	amessage = aclient->reading_queue.front();
	for (auto achannel = channels.begin(); achannel != channels.end(); ++achannel)
	{
		if (achannel->get_name() == amessage.get_content_string())
		{
			achannel->remove_client(aclient->get_sock());
			break;
		}
	}
}

void Server::process_createchan(std::vector<Client>::iterator aclient)
{
	Message amessage;

	amessage = aclient->reading_queue.front();
	add_channel(amessage.get_content(), aclient->get_sock());
}

void Server::add_channel(const char *name, int client)
{
	Channel channel(name, client);

	std::cout << "Channel creation (" << name << ")" << std::endl;
	channels.push_back(channel);
}

void Server::process_deletechan(Message message)
{
	for (auto achannel = channels.begin(); achannel != channels.end(); ++achannel)
		if (achannel->get_name() == message.get_content_string())
		{
			channels.erase(achannel);
			break;
		}
}

void Server::on_write()
{
	for(auto a_client = clients.begin(); a_client != clients.end(); ++a_client)
	{
		if (FD_ISSET(a_client->get_sock(), &write_sockets))
			a_client->write();
	}
}

void Server::update_clients()
{
	auto client = clients.begin();

	while (client != clients.end())
	{
		if (client->get_sock() < 0)
		{
			std::cout << "Removing " << client->get_name() << std::endl;
			clients.erase(client);
		}
		else
			++client;
	}
}

void Server::process_channel_message()
{
	for (auto channel = channels.begin(); channel != channels.end(); ++channel)
	{
		while(!channel->writing_queue.empty())
		{
			Message amessage = channel->writing_queue.front();
			channel->writing_queue.pop();
			for (auto client = clients.begin(); client != clients.end(); ++client)
				client->writing_queue.push(amessage);
		}
	}
}

void Server::on_cleanup()
{
	if (!clients.empty())
	{
		for (auto aclient : clients)
			close(aclient.get_sock());
		clients.clear();
	}
	if (!shutdown(main_socket, SHUT_RDWR))
		std::cout << "Shutdown successed." << std::endl;
	else
	{
		std::cerr << "Impossible to shutdown main socket" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Server::check_running(int signal)
{
	if (signal == SIGINT || signal == SIGTERM)
		g_running = false;
}
