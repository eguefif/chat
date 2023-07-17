#include "server.h"

void to_non_blocking(int sock)
{
	int flags;

	flags = fcntl(sock, F_GETFD);
	if (fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		std::cerr << "Impossible to set socket " << sock << " to non blocking." << std::endl;
		exit(EXIT_FAILURE);
	}
}

std::string trim(std::string str)
{
	std::string whitespace = "\n\r\t\v\f ";
	str.erase(str.find_last_not_of(whitespace) + 1);
	str.erase(0, str.find_first_not_of(whitespace));
	return (str);
}
