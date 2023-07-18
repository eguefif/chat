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
{}

const char *Client::get_address()
{
	return (addr.c_str());
}

int Client::get_id()
{
	return (id);
}
