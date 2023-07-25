#include "server.h"

Channel::Channel(const char *aname, int asock) :
	name(aname)
{
	clients.push_back(asock);
}


void Channel::add_client(int asock)
{
	clients.push_back(asock);
}

void Channel::remove_client(int asock)
{
	for (auto aclient = clients.begin(); aclient != clients.end(); ++aclient)
	{
		if (*aclient == asock)
		{
			clients.erase(aclient);
			break;
		}
	}
}

void Channel::add_message(Message amessage)
{
	for (auto aclient = clients.begin(); aclient != clients.end(); ++aclient)
		writing_queue.push(amessage);
}

std::string Channel::get_name()
{
	return (name);
}
