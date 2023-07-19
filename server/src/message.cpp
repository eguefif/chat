#include "server.h"

Message::Message(const char *acommand, const char *acontent) :
	command(acommand),
	content(acontent)
{}

Message::Message(const char *acommand, std::string acontent) :
	command(acommand),
	content(acontent)
{}

void Message::set_command(const char *acommand)
{
	command = acommand;
}

void Message::set_content(const char *acontent)
{
	content = acontent;
}

int Message::get_command()
{
	if (command == "chat")
		return CHAT;
	else if (command == "name")
		return NAME;
	else if (command == "list")
		return LIST;
	else if (command == "EOC")
		return ENDCONN;
	return FAILED;
}

const char *Message::get_content()
{
	return (content.c_str());
}

size_t Message::get_size()
{
	return content.size();
}
