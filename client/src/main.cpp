#include "client.h"

int main()
{
	Client client("Emmanuel");

	std::cout << "Name" << client.get_name() << std::endl;
	return (0);
}
