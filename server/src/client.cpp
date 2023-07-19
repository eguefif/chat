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
	//std::cout << "Begining the loop for client message number: " << writing_queue.size() << std::endl;
	while (!writing_queue.empty())
	{
		auto message = writing_queue.front();
		std::cout << "Sending to " << name << " : \n" << message.get_content() << std::endl;
		int ret = send(sock, message.get_content(), message.get_size(), 0);
		writing_queue.pop();
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
