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
#define NAME_SIZE 20
#define CLOSING_CONNEXION 0
#define SUCCESS 1
#define FAILED -1
#define CHAT 0
#define NAME 1
#define LIST 2
#define EOC 3
#define JOIN_CHAN 4
#define CREATE_CHAN 5
#define LEAVE_CHAN 6
#define DELETE_CHAN 7
#define TODELETE -1
#define DST_SIZE 20

extern bool g_running;


class Message
{
public:
	Message() = default;
	Message(std::string acommand, std::string acontent, const char *asrc);
	Message(std::string acommand, std::string acontent);
	Message(std::string message);
	void set_command(const char *acommand);
	void set_content(const char *acontent);
	int get_command();
	size_t get_content_size();
	size_t get_message_size();
	const char *get_content();
	std::string get_content_string();
	const char *get_message();
	std::string get_command_verbose();
	std::string get_dst();
	bool is_send();

private:
	std::string command;
	std::string content;
	std::string message;
	std::string dst;
	std::string src;
	bool sent = false;
	bool is_command();
};

class Channel
{
public:
	Channel(const char *name, int asock);
	void add_client(int asock);
	void remove_client(int asock);
	void add_message(Message amessage);
	std::string get_name();
	std::queue<Message> writing_queue;
	
private:
	std::vector<int> clients;
	std::string name;
};

class Protocol
{
public:
	Protocol() = default;
	Protocol(int asocket);
	Protocol(int asocket, Message amessage);
	int onread();
	void write();
	Message get_message();

private:
	int sock;
	int message_size;
	char protoheader[PROTOHEADER_SIZE];
	char temp_message[MAX_MESSAGE];
	Message message;
};


class Client
{
public:
	Client(int a_sock, int an_id);
	int get_sock();
	const char *get_address();
	int onread();
	void write();
	int get_id();
	void set_sock(int asock);
	std::string get_name();
	void set_name(std::string aname);
	std::queue<Message> reading_queue;
	std::queue<Message> writing_queue;

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
	std::vector<Channel> channels;
	int	current_id = 0;

	void on_init();
	void on_serve();
	void on_listen();
	void on_process_message();
	void on_write();
	void add_client();
	void on_cleanup();
	void update_clients();
	void process_list(std::vector<Client>::iterator aclient);
	void process_chat(std::vector<Client>::iterator aclient);
	void process_name(std::vector<Client>::iterator aclient);
	void process_leavechan(std::vector<Client>::iterator aclient);
	void process_joinchan(std::vector<Client>::iterator aclient);
	void process_createchan(std::vector<Client>::iterator aclient);
	void process_deletechan(Message message);
	void add_channel(const char *name, int client);
	void process_channel_message();
	void delete_client(std::vector<Client>::iterator aclient);
	static void check_running(int signal);
	int	get_highest_sock_number();
	void init_fdsets();
	Message build_message_echo(Message src, std::vector<Client>::iterator aclient);
	void notify_all_client(std::string name);
};

void to_non_blocking(int sock);
std::string trim(std::string str);
