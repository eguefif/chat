#include "client.h"

bool g_running = true;

int main(int argc, char *argv[])
{
	Client client;

	if (argc == 2)
	{
		std::cout << "Test\n";
		client = Client(argv[1]);
	}
	else if (argc == 4)
		client = Client(argv[1], argv[2], atoi(argv[3]));

	client.init();
	client.run();
	client.cleanup();
	return (0);
}
