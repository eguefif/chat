#include "client.h"

Message::Message(std::string acommand, std::string adst, std::string acontent) :
	command(acommand),
	dst(adst),
	content(acontent)
{
	char temp[MAX_MESSAGE];
	int size = 24;

	size += acontent.size();
	sprintf(temp, "%05dchat%-20s%s", size, dst.c_str(), content.c_str());
	message = temp;
}

Message::Message(std::string acommand, std::string acontent) :
	command(acommand),
	content(acontent)
{
	char temp[MAX_MESSAGE];
	int size = 24;

	size = command.size() + content.size();
	sprintf(temp, "%05d%s%s", size, command.c_str(), content.c_str());
	message = temp;
}

const char *Message::get_message()
{
	return (message.c_str());
}

size_t Message::get_size()
{
	return (message.size());
}
