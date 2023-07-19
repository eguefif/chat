#include "server.h"

Protocol::Protocol(int asocket) :
	sock(asocket)
{}

Protocol::Protocol(int asocket, Message amessage) :
	sock(asocket)
{
	message = amessage;
}

int Protocol::onread()
{
	int retvalue;

	memset(protoheader, 0, PROTOHEADER_SIZE);
	memset(temp_message, 0, MAX_MESSAGE);
	retvalue = read(sock, protoheader, PROTOHEADER_SIZE);
	if (retvalue == FAILED)
		return (FAILED);
	message_size = atoi(protoheader);
	read(sock, temp_message, message_size);
	Message amessage(temp_message);
	message = amessage;
	return (SUCCESS);
}

void Protocol::write()
{
	std::cout << "Sending: " << message.get_message() << std::endl;
	send(sock, message.get_message(), message.get_message_size(), 0);
}

Message Protocol::get_message()
{
	return (message);
}
