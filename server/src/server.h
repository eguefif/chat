#pragma once
#include <iostream>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <csignal>
#include <vector>
#include <unistd.h>
#define COMMAND_SIZE 4 
#define PROTOHEADER_SIZE 5
#define MAX_MESSAGE 10000
#define CLOSING_CONNEXION 0
#define SUCCESS 1
#define FAILED -1

extern bool g_running;

class Protocol
{
public:
	Protocol() = default;
	Protocol(int asocket);
	int onread();
	void write();
	const char *get_command();
	const char *get_message();

private:
	int sock;
	int message_size;
	std::string command;
	std::string message;

	void save_command(char temp[]);
	void save_message(char temp[]);
};


class Client
{
public:
	Client(int a_sock, int an_id);
	int get_sock();
	int onread();
	void write();
	const char *get_address();

private:
	int sock;
	int id;
	sockaddr_in s_addr;
	std::string name;
	std::string addr;
};


class Server
{
public:
	Server() = default;
	Server(std::string anAddress, int aPort);
	void run();

private:
	std::string address = "127.0.0.1";
	int port = 10000;
	int	main_socket;
	struct sockaddr_in server_address;
	fd_set read_sockets;
	fd_set write_sockets;
	struct timeval tv;
	std::vector<Client> clients;
	int	current_id = 0;

	void on_init();
	void on_serve();
	void on_listen();
	void on_transfer_message();
	void on_write();
	void add_client();
	void on_cleanup();
	void delete_client(Client aclient);
	static void check_running(int signal);
	int	get_highest_sock_number();
	void init_fdsets();
};

void to_non_blocking(int sock);
std::string trim(std::string str);
