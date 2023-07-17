#include "server.h"

Message::Message(std::string acommand, std::string acontent) :
	command(acommand),
	content(acontent)
{}

void Message::set_command(std::string acommand)
{
	command = acommand;
}

void Message::set_content(std::string acontent)
{
	content = acontent;
}

const char *Message::get_command()
{
	return (command.c_str());
}

const char *Message::get_content()
{
	return (content.c_str());
}
