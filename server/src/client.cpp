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
	std::cout << "New client " << addr.c_str() << "(" << sock << ")" << std::endl;
	name = "Unknown";
	Connexion acommunication(sock);
	communication = acommunication;
}

int Client::get_sock()
{
	return (sock);
}

int Client::onread()
{
	int retvalue;
	Message amessage;

	retvalue = communication.onread();
	if(retvalue == FAILED)
		return (FAILED);
	if (communication.is_message_ready())
	{
		amessage = communication.get_message();
		reading_queue.push(amessage);
		std::cout << "Receiving message " << amessage.get_message() << std::endl;
		communication.reset();
	}
	return (SUCCESS);
}

void Client::write()
{
	while (!writing_queue.empty())
	{
		auto message = writing_queue.front();
		Connexion connexion(sock, message);
		connexion.write();
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
	return (name);
}

void Client::set_name(std::string aname)
{
	name = aname;
}
