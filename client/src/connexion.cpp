#include "client.h"

Connexion::Connexion(std::string anaddress, int aport) :
	address(anaddress),
	port(aport)
{}

int Connexion::init()
{
	std::string message1 = "Welcome to the chat.";
	std::string message2 = "Type \\h help";
	std::string message3 = "Type \\l to get the list of connected users and channels.";
	std::string message4 = "Type \\c + name to create a channel.";
	std::string message5 = "Type \\e + name to exit a channel.";
	std::string message6 = "Type \\j + name to join a channel.";
	std::string message7 = "Type \\q to exit.";

	messages.push_back(message1);
	messages.push_back(message2);
	messages.push_back(message3);
	messages.push_back(message4);
	messages.push_back(message5);
	messages.push_back(message6);
	messages.push_back(message7);

	init_connexion();
	return (1);
}

void Connexion::init_connexion()
{
	int opt = 1;
	struct sockaddr_in server_address;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	to_non_blocking(fd);
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(port);
	server_address.sin_addr.s_addr = inet_addr(address.c_str());
	memset(server_address.sin_zero, 0, sizeof(server_address.sin_zero));
	connect(fd, (struct sockaddr *) &server_address, sizeof(server_address));
	tv.tv_sec = 0;
	tv.tv_usec = 50;
}

void Connexion::to_non_blocking(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFD);
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Impossible to set socket " << fd << " to non blocking." << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Connexion::send_name(std::string name)
{
	int size;
	char temp[MAX_MESSAGE];
	std::string message;

	message = "name" + name;
	sprintf(temp, "%05d%s", (int) message.size(), message.c_str());
	send(fd, temp, strlen(temp), 0);
}

bool Connexion::is_there_new_messages()
{
	return (new_message);
}

void Connexion::set_new_message_to_false()
{
	new_message = false;
}

std::vector<std::string> Connexion::get_messages()
{
	return (messages);
}

void Connexion::add_message(std::string message)
{
	messages.push_back(message);
}

void Connexion::flush_new_messages()
{
	messages.clear();
}

std::string Connexion::get_stdin_buffer()
{
	return (stdin_buffer);
}

void Connexion::flush_stdin_buffer()
{
	stdin_buffer.clear();
}

void Connexion::set_stdin_buffer(std::string buffer)
{
	stdin_buffer = buffer;
}

void Connexion::update()
{
	int retval;

	init_fdsets();
	retval = select(fd + 1,
			&read_sockets,
			&write_sockets,
			NULL, &tv);
	if (retval > 0)
	{
		write();
		listen();
	}

}

void Connexion::init_fdsets()
{
	FD_ZERO(&read_sockets);
	FD_SET(fd, &read_sockets);
	FD_SET(STDIN_FILENO, &read_sockets);
	FD_SET(fd, &write_sockets);
}

void Connexion::write()
{
	if (FD_ISSET(fd, &write_sockets))
	{
		while (!sending_queue.empty())
		{
			Message message = sending_queue.front();
			sending_queue.pop();
			send(fd, message.get_message(), message.get_size(), 0);
		}
	}
}

void Connexion::listen()
{
	char c;

	if (FD_ISSET(STDIN_FILENO, &read_sockets))
	{
		read(STDIN_FILENO, &c, 1); 
		stdin_buffer.push_back(c);
		new_stdin_entry = true;
	}
	if(FD_ISSET(fd, &read_sockets))
		get_server_message();
}

void Connexion::get_server_message()
{
	char protoheader[PROTOHEADER_SIZE];
	char content[MAX_MESSAGE];
	int size;


	memset(content, '\0', MAX_MESSAGE);
	memset(protoheader, '\0', PROTOHEADER_SIZE);
	read(fd, protoheader, 5);
	size = atoi(protoheader);
	read(fd, content, size);
	process_message(content);
}

void Connexion::process_message(std::string content)
{
	std::string message = content;
	std::string command = content.substr(0, 4);

	if (command == "list")
		add_list(content.substr(4, content.size() - 4));
	else if (command == "EOC")
	{
		std::cerr << "Server ended connexion." << std::endl;
		g_running = false;
	}
	else if (command == "chat")
		add_message_from_socket(content.substr(4, content.size() -4));
}

void Connexion::add_list(std::string list)
{
	std::string name = "";

	messages.push_back("List of connected users");
	for(auto c = list.begin(); c != list.end(); ++c)
	{
		if (*c == '\0')
			break;
		else if (*c == ' ')
		{
			messages.push_back(name);
			name = "";
		}
		else
			name.push_back(*c);
	}
	new_message = true;
}

void Connexion::add_message_from_socket(std::string message)
{
	std::string src;
	std::string content;
	size_t pos;

	pos = message.find_first_of(" ");
	src = message.substr(0, pos);
	content = message.substr(20, message.size() - 20);
	messages.push_back(src + ": " + content);
	new_message = true;
}

void Connexion::send_message(std::string entry)
{
	size_t limit;
	std::string dst;
	std::string content;

	limit = entry.find_first_of(" ");
	dst = entry.substr(0, limit);
	content = entry.substr(limit + 1, entry.size() - limit - 1);
	Message message("chat", dst, content);
	sending_queue.push(message);
}

void Connexion::send_command(std::string command, std::string entry)
{
	size_t limit;
	std::string content;

	limit = entry.find_first_of(" ");
	content = entry.substr(limit + 1, entry.size() - limit - 1);
	Message message(command, content);
	std::cerr << "sending: " << message.get_message() << std::endl;
	sending_queue.push(message);
}

void Connexion::request_list()
{
	send(fd, "00004list", 9, 0);
}

void Connexion::cleanup()
{
	send(fd, "00003EOC", 8, 0);
	close(fd);
}

bool Connexion::is_stdin_ready()
{
	return (new_stdin_entry);
}

void Connexion::switch_stdin_entry_to_false()
{
	new_stdin_entry = false;
}
