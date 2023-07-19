#include "server.h"

Client::Client(int a_sock, int an_id) :
	sock(a_sock),
	id(an_id)
{
	char dst[INET_ADDRSTRLEN];

	getsockname(a_sock,
			(struct sockaddr *) &s_addr,
			(socklen_t *) sizeof(s_addr));
	addr = inet_ntop(AF_INET, &(s_addr.sin_addr), dst, INET_ADDRSTRLEN);
	addr = inet_ntop(AF_INET, &(s_addr.sin_addr), dst, INET_ADDRSTRLEN);
	std::cout << "New client " << addr.c_str() << std::endl;
	name = "Unknown";
}

int Client::get_sock()
{
	return (sock);
}

int Client::onread()
{
	Protocol communication(get_sock());
	int retvalue;

	retvalue = communication.onread();
	if(retvalue == FAILED)
		return (FAILED);
	Message amessage(communication.get_command(), communication.get_content());
	if (amessage.get_command() == ENDCONN)
	{
		std::cout << "Received end of connexion for client (" << addr << ")" << std::endl;
		return (CLOSING_CONNEXION);
	}
	reading_queue.push(amessage);
	return (SUCCESS);
}

void Client::write()
{
	while (!writing_queue.empty())
	{
		auto message = writing_queue.front();
		std::cout << "sending to " << name << " this: " << message.get_content() << std::endl;
		if (send(sock, message.get_content(), message.get_size(), 0) != -1)
		{
			writing_queue.pop();
			std::cout << "poping message now" << writing_queue.size () <<std::endl;
		}
		else
		{
			std::cerr << "Impossible to send message to " << name << "(" << addr << ")" << std::endl;
			std::cerr << "Content: \n" << message.get_content() << std::endl;
		}
	}
}

const char *Client::get_address()
{
	return (addr.c_str());
}

int Client::get_id()
{
	return (id);
}

void Client::set_sock(int asock)
{
	sock = asock;
}

std::string Client::get_name()
{
	return name;
}

void Client::set_name(std::string aname)
{
	name = aname;
}
