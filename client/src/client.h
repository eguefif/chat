#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <queue>
#include <vector>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#define MAX_MESSAGE 10000
#define PROTOHEADER_SIZE 5
#define BACKSPACE 127
#define ENTRY 10
#define BACKSLASH 92

extern bool g_running;

class Message
{
public:
	Message(std::string adst, std::string acontent);
	const char *get_message();
	size_t get_size();

private:
	std::string dst;
	std::string content;
	std::string message;
};

class Command
{
public:
	Command(std::string acommand, std::string acontent);
	const char *get_message();
	size_t get_size();

private:
	std::string command;
	std::string content;
	std::string message;
};

class Connexion
{
public:
	Connexion() = default;
	Connexion(std::string anaddress, int aport);
	int init();
	bool is_there_new_messages();
	void set_new_message_to_false();
	std::vector<std::string> get_messages();
	void flush_new_messages();
	std::string get_stdin_buffer();
	void flush_stdin_buffer();
	void set_stdin_buffer(std::string buffer);
	void update();
	void send_message(std::string entry);
	void add_message(std::string message);
	void cleanup();
	void request_list();
	void send_name(std::string name);
	bool is_stdin_ready();
	void switch_stdin_entry_to_false();

private:
	int fd;
	bool new_message = true;
	bool new_stdin_entry = false;;
	std::string address = "127.0.0.1";
	int port = 10000;
	std::vector<std::string> messages;
	std::string stdin_buffer;
	std::queue<Message> sending_queue;
	struct timeval tv;
	fd_set read_sockets;
	fd_set write_sockets;

	void init_connexion();
	void to_non_blocking(int fd);
	void write();
	void listen();
	void init_fdsets();
	void get_server_message();
	void process_message(std::string content);
	void add_list(std::string list);
	void add_message_from_socket(std::string message);
};

class Display
{
public:
	Display() = default;
	int init();
	void refresh(std::string stdin_buffer, std::vector<std::string> messages);
	void cleanup();

private:
	struct termios orig_termios;
	std::string buffer = "";
	int cols = 30;
	int rows = 10;
	int x;
	int y;

	void print_to_screen();
	void disablerawmode();
	void display_rows();
	void update_size();
	void clear_screen();
	void echo_stdin(std::string buffer);
	void display_messages(std::vector<std::string> messages);
	void show_cursor();
	void hide_cursor();
	void move_cursor_stdin();
	void move_cursor_message();
};

class Client
{
public:
	Client() = default;
	Client(std::string aname);
	Client(std::string aname, std::string anaddress, int aport);
	int init();
	int run();
	void cleanup();
	const char *get_name();

private:
	std::string name = "None";
	std::string address = "127.0.0.1";
	int port = 10000;
	Display screen;
	Connexion connexion;
	std::vector<std::string> messages;
	std::string stdin_buffer;
	bool is_display = true;

	void process_stdin();
	void update();
	void update_messages();
	void update_stdin_buffer();
	void display();
	void check_command();
	void send_message();
	void help();
	void create_channel();
	void exit_channel();
	void join_channel();
	void delete_channel();
};

void signal_handler(int signal);
