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
#include <queue>
#define COMMAND_SIZE 4 
#define PROTOHEADER_SIZE 5
#define MAX_MESSAGE 10000
#define CLOSING_CONNEXION 0
#define SUCCESS 1
#define FAILED -1
#define CHAT 0
#define NAME 1
#define LIST 2
#define ENDCONN 3

extern bool g_running;

class Message
{
public:
	Message() = default;
	Message(const char *acommand, const char *acontent);
	void set_command(const char *acommand);
	void set_content(const char *acontent);
	int get_command();
	const char *get_content();

private:
	std::string command;
	std:: string content;
};

class Protocol
{
public:
	Protocol() = default;
	Protocol(int asocket);
	int onread();
	void write();
	const char *get_command();
	const char *get_content();

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
	int get_id();
	std::queue<Message> reading_queue;

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
	void on_process_message();
	void on_write();
	void add_client();
	void on_cleanup();
	void delete_client(int sock);
	static void check_running(int signal);
	int	get_highest_sock_number();
	void init_fdsets();
};

void to_non_blocking(int sock);
std::string trim(std::string str);
