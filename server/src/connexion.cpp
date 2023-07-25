#include "server.h"

Connexion::Connexion(int asocket) :
	sock(asocket)
{

	memset(protoheader, 0, PROTOHEADER_SIZE);
	memset(temp_message, 0, MAX_MESSAGE);
}

Connexion::Connexion(int asocket, Message amessage) :
	sock(asocket)
{
	message = amessage;
	memset(protoheader, 0, PROTOHEADER_SIZE);
	memset(temp_message, 0, MAX_MESSAGE);
}

int Connexion::onread()
{
	int retvalue;

	if (!is_protoheader_read())
		if (read_protoheader() == FAILED)
			return (FAILED);
	if (is_protoheader_read())
		if (read_message() == FAILED)
			return (FAILED);
	return (SUCCESS);
}

int Connexion::read_protoheader()
{
	char temp[PROTOHEADER_SIZE];
	int retvalue;

	memset(temp, 0, PROTOHEADER_SIZE);
	retvalue = read(sock, temp, PROTOHEADER_SIZE - protoheader_read);
	if (retvalue == FAILED)
		return (FAILED);
	strncat(protoheader, temp, retvalue);
	protoheader_read += retvalue;
	if (is_protoheader_read())
		message_size = atoi(protoheader);
	return (SUCCESS);
}

int Connexion::read_message()
{
	int retvalue;
	char temp[MAX_MESSAGE];

	memset(temp, 0, MAX_MESSAGE);
	retvalue = read(sock, temp_message, message_size - message_read);
	if (retvalue == FAILED)
		return (FAILED);
	strncat(temp_message, temp, retvalue);
	message_read += retvalue;
	std::cout << "Message size : " << message_size << std::endl;
	if (message_read == message_size)
	{
		Message amessage(temp_message);
		message = amessage;
		message_ready = true;
	}
	return (SUCCESS);
}

bool Connexion::is_protoheader_read()
{
	if (protoheader_read == 5)
		return true;
	return false;
}

bool Connexion::is_message_read()
{
	if (message_size != 0)
		if (message_size == message_read)
			return (true);
	return (false);
}

void Connexion::write()
{
	std::cout << "Sending: " << message.get_message() << std::endl;
	send(sock, message.get_message(), message.get_message_size(), 0);
}

Message Connexion::get_message()
{
	return (message);
}

bool Connexion::is_message_ready()
{
	return (message_ready);
}

void Connexion::reset()
{
	protoheader_read = 0;
	message_read = 0;
	message_ready = false;
	message_size = 0;
	memset(protoheader, 0, PROTOHEADER_SIZE);
	memset(temp_message, 0, MAX_MESSAGE);
}

bool Connexion::is_processing_message()
{
	if (message_ready)
		return false;
	if (protoheader_read > 0 && protoheader_read < 4)
		return true;
	if (message_read > 0)
		return true;
	return false;
}
