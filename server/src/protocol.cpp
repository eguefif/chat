#include "server.h"

Protocol::Protocol(int asock) :
	sock(asock)
{}

int Protocol::onread()
{
	char protoheader[PROTOHEADER_SIZE];
	char temp_message[MAX_MESSAGE];
	int retvalue;

	command.clear();
	message.clear();
	memset(protoheader, 0, PROTOHEADER_SIZE);
	memset(temp_message, 0, MAX_MESSAGE);
	retvalue = read(sock, protoheader, PROTOHEADER_SIZE);
	if (retvalue == CLOSING_CONNEXION)
		return (CLOSING_CONNEXION);
	else if (retvalue == FAILED)
		return (FAILED);
	message_size = atoi(protoheader);
	read(sock, temp_message, message_size);
	save_command(temp_message);
	save_message(&temp_message[COMMAND_SIZE]);
	return (SUCCESS);
}

void Protocol::save_command(char temp[])
{
	char temp_command[COMMAND_SIZE];

	memset(temp_command, 0, COMMAND_SIZE);
	strncpy(temp_command, temp, COMMAND_SIZE);
	command = trim(temp_command);
}

void Protocol::save_message(char temp[])
{
	char temp_message[MAX_MESSAGE];

	memset(temp_message, 0, MAX_MESSAGE);
	strcpy(temp_message, temp);
	message = temp_message;
}

void Protocol::write()
{}

const char *Protocol::get_command()
{
	return command.c_str();
}

const char *Protocol::get_content()
{
	return message.c_str();
}
