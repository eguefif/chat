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
		if (stdin_buffer.size() > 1)
		{
			stdin_buffer.pop_back();
			stdin_buffer.pop_back();
			connexion.set_stdin_buffer(stdin_buffer);
			is_display = true;
		}
	}
	if (stdin_buffer.back() == ENTRY)
	{
		stdin_buffer.pop_back();
		if (stdin_buffer.front() == BACKSLASH)
			check_command();
		else
			send_message();
		connexion.flush_stdin_buffer();
		stdin_buffer.clear();
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
		case 'c': create_channel();
				  break;
		case 'e': exit_channel();
				  break;
		case 'j': join_channel();
				  break;
		case 'd': delete_channel();
				  break;
		case 'n': rename();
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
	std::cout << "Bye bye " << name << std::endl;
}

void Client::help()
{
	std::string message0 = "Help";
	std::string message1 = "Type \\l to get the list of connected users and channels.";
	std::string message2 = "Type \\c + name to create a channel.";
	std::string message3 = "Type \\e + name to exit a channel.";
	std::string message4 = "Type \\j + name to join a channel.";
	std::string message5 = "Type \\q to exit.";

	messages.push_back(message0);
	messages.push_back(message1);
	messages.push_back(message2);
	messages.push_back(message3);
	messages.push_back(message4);
	messages.push_back(message5);
}

void Client::create_channel()
{
	connexion.send_command("crea", stdin_buffer);
}

void Client::exit_channel()
{
	connexion.send_command("exit", stdin_buffer);
}

void Client::join_channel()
{
	connexion.send_command("join", stdin_buffer);
}

void Client::delete_channel()
{
	connexion.send_command("dele", stdin_buffer);
}

void Client::rename()
{
	size_t limit;

	limit = stdin_buffer.find_first_of(" ");
	name = stdin_buffer.substr(limit + 1, stdin_buffer.size() - limit -1);
	connexion.send_name(name);
}

