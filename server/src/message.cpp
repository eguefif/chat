#include "server.h"

Message::Message(std::string acommand, std::string acontent, const char *asrc)
{
	char amessage[MAX_MESSAGE];
	int size = 0;

	command = acommand;
	content = acontent;
	src = asrc;
	size += command.size();
	size += content.size();
	size += NAME_SIZE;
	sprintf(amessage, "%05d%s%-*s%s", size, command.c_str(), NAME_SIZE, src.c_str(), content.c_str());
	message = amessage;
}

Message::Message(std::string acommand, std::string acontent)
{
	char amessage[MAX_MESSAGE];
	int size = 0;

	command = acommand;
	content = acontent;
	size += command.size();
	size += content.size();
	sprintf(amessage, "%05d%s%s", size, command.c_str(), content.c_str());
	message = amessage;
}

Message::Message(std::string amessage)
{
	message = amessage;
	command = message.substr(0, 4);
	if (command == "chat")
	{
		dst = trim(message.substr(4, NAME_SIZE));
		content = trim(message.substr(24, message.size() - 24));
	}
	else if (is_command())
		content = message.substr(4, message.size() - 4);
	else
		content = "";
}

bool Message::is_command()
{
	if (command == "name" || command == "crea" || command == "exit" || command == "join" || command == "dele")
		return true;
	return false;
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
	if (command == "chat")
		return CHAT;
	else if (command == "name")
		return NAME;
	else if (command == "list")
		return LIST;
	else if (command == "EOC")
		return EOC;
	else if (command == "crea")
		return CREATE_CHAN;
	else if (command == "join")
		return JOIN_CHAN;
	else if (command == "exi")
		return LEAVE_CHAN;
	else if (command == "dele")
		return DELETE_CHAN;
	return FAILED;
}

std::string Message::get_command_verbose()
{
	return (command);
}

const char *Message::get_content()
{
	return (content.c_str());
}

std::string Message::get_content_string()
{
	return (content);
}

size_t Message::get_content_size()
{
	return (content.size());
}

const char *Message::get_message()
{
	return (message.c_str());
}

size_t Message::get_message_size()
{
	return (message.size());
}

std::string Message::get_dst()
{
	return (dst);
}
