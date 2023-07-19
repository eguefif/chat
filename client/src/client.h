#pragma once
#include <iostream>
#include <string>


class Client
{
public:
	Client() = default;
	Client(std::string aname);
	const char *get_name();

private:
	std::string name = "None";
};
