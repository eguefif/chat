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
		if (retval == -1)
			std::cerr << "Impossible to use select" << std::endl;
		else if (retval > 0)
		{
			if (FD_ISSET(main_socket, &read_sockets))
			{
				add_client();
			}
			on_listen();
			on_process_message();
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
		FD_SET(aclient.get_sock(), &read_sockets);
		FD_SET(aclient.get_sock(), &write_sockets);
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
			if (FD_ISSET((*a_client).get_sock(), &read_sockets))
			{
				retvalue = (*a_client).onread();
				if (retvalue == FAILED)
					std::cerr << "Problem while reading socket " << std::endl;
				else if (retvalue == CLOSING_CONNEXION)
					delete_client(a_client);
			}
		}
	}
}

void Server::delete_client(std::vector<Client>::iterator aclient)
{
	std::cout << "Deleting client : " << aclient->get_name().c_str() << std::endl;
	close(aclient->get_sock());
	aclient->set_sock(TODELETE);
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
	aclient->set_name(amessage.get_content());
}

void Server::process_list(std::vector<Client>::iterator aclient)
{
	std::string content = "list";
	char protoheader[PROTOHEADER_SIZE];
	int size;

	for (auto aclient = clients.begin(); aclient != clients.end(); ++aclient)
		content = content + aclient->get_name();
	size = content.size();
	sprintf(protoheader, "%05d", size);
	content = protoheader + content;
	Message message("list", content);
	aclient->writing_queue.push(message);
}

void Server::process_chat(std::vector<Client>::iterator aclient)
{
}

void Server::on_write()
{
	for(auto a_client = clients.begin(); a_client != clients.end(); ++a_client)
	{
		if (FD_ISSET(a_client->get_sock(), &write_sockets))
			a_client->write();
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
