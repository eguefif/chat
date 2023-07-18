#include "server.h"

Message::Message(const char *acommand, const char *acontent) :
	command(acommand),
	content(acontent)
{
}

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
	if (strcmp(command, "chat"))
		return CHAT;
	else if (strcmp(command, "name"))
		return NAME;
	else if (strcmp(command, "list"))
		return LIST;
	return FAILED;
}

const char *Message::get_content()
{
	return (content);
}
