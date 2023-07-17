#include "server.h"

bool g_running = true;

int main(int argc, char *argv[])
{
	Server		server;
	std::string address;
	int			port;

	if (argc == 3)
	{
		address = argv[1];
		port = atoi(argv[2]);
		server = Server();
		server.run();
	}
	else
	{
		server = Server();
		server.run();
	}
	return (1);
}
