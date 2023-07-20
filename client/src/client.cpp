#include "client.h"

Client::Client(std::string aname) :
	name(aname)
{}

Client::Client(std::string aname, std::string anaddress, int aport) :
	name(aname),
	address(anaddress),
	port(aport)
{}

const char *Client::get_name()
{
	return (name.c_str());
}

int Client::init()
{
	Connexion aconnexion(address, port);

	connexion = aconnexion;
	if (connexion.init() < 0)
	{
		std::cerr << "Impossible to init connexion" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (screen.init() < 0)
	{
		std::cerr << "Impossible to init display" << std::endl;
		exit(EXIT_FAILURE);
	}
	signal(SIGTERM, signal_handler);
	signal(SIGINT, signal_handler);
	connexion.send_name(name);
	return (1);
}

int Client::run()
{
	while (g_running)
	{
		connexion.update();
		update();
		process_stdin();
		display();
	}

	return (1);
}

void Client::update()
{
	update_messages();
	update_stdin_buffer();
}

void Client::update_messages()
{
	std::vector<std::string> new_messages;

	if (connexion.is_there_new_messages())
	{
		new_messages = connexion.get_messages();
		connexion.flush_new_messages();
		for(auto new_message : new_messages)
			messages.push_back(new_message);
		connexion.set_new_message_to_false();
		is_display = true;
	}
}

void Client::update_stdin_buffer()
{
	if (connexion.is_stdin_ready())
	{
		stdin_buffer = connexion.get_stdin_buffer();
		connexion.switch_stdin_entry_to_false();
		is_display = true;
	}
}

void Client::process_stdin()
{
	if (stdin_buffer.back() == BACKSPACE)
	{
		stdin_buffer.pop_back();
		stdin_buffer.pop_back();
		connexion.set_stdin_buffer(stdin_buffer);
	}
	if (stdin_buffer.back() == ENTRY)
	{
		stdin_buffer.pop_back();
		if (stdin_buffer.front() == BACKSLASH)
			check_command();
		else
			send_message();
		connexion.flush_stdin_buffer();
		is_display = true;
	}
}

void Client::check_command()
{
	switch (stdin_buffer.at(1))
	{
		case 'h': help();
				  break;
		case 'l': connexion.request_list();
				  break;
		case 'q': g_running = false;
				  break;
		default:
				  break;
	}
}

void Client::send_message()
{
	connexion.send_message(stdin_buffer);
}


void Client::display()
{
	if (is_display)
	{
		screen.refresh(stdin_buffer, messages);
		is_display = false;
	}
}

void Client::cleanup()
{
	connexion.cleanup();
	screen.cleanup();
}

void Client::help()
{
	std::string help = "HELP - \\q : exit the chat, \\l : request list of user, USER message";
	connexion.add_message(help);
}
