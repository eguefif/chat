#include "client.h"

Client::Client(std::string aname) :
	name(aname)
{}

const char *Client::get_name()
{
	return (name.c_str());
}
