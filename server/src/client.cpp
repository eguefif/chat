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
	Protocol message(get_sock());
	int retvalue;

	retvalue = message.onread();
	if (retvalue == CLOSING_CONNEXION)
		return (CLOSING_CONNEXION);
	else if(retvalue == FAILED)
		return (FAILED);
	std::cout << "Command: " << message.get_command() << std::endl;
	std::cout << "Message: " << message.get_message() << std::endl;
	return (SUCCESS);
}

void Client::write()
{}

const char *Client::get_address()
{
	return (addr.c_str());
}
