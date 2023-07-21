#include "client.h"

int Display::init()
{
	struct termios raw_config;

	tcgetattr(STDIN_FILENO, &orig_termios);
	raw_config = orig_termios;
	raw_config.c_lflag &= ~(ECHO | ICANON);
	//raw_config.c_oflag &= ~(OPOST);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_config);
	
	clear_screen();
	update_size();

	return (1);
}

void Display::clear_screen()
{
	buffer += "\x1b[2J";
}

void Display::disablerawmode()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void Display::refresh(std::string stdin_buffer, std::vector<std::string> messages)
{
	buffer.clear();
	clear_screen();
	update_size();
	display_rows();
	display_messages(messages);
	echo_stdin(stdin_buffer);
	hide_cursor();
	print_to_screen();
	show_cursor();
}

void Display::hide_cursor()
{
	buffer += "\x1b[?25l";
}

void Display::show_cursor()
{
	buffer += "\x1b[?25h";
}

void Display::echo_stdin(std::string stdin_buffer)
{
	char cursor[500];

	x = buffer.size() + 1;
	y = rows;
	move_cursor_stdin();
	buffer += "->";
	buffer += stdin_buffer;
}


void Display::display_messages(std::vector<std::string> messages)
{
	auto display_messages = messages;
	std::vector<std::string>::iterator first;
	std::vector<std::string>::iterator last;
	char cursor[500];

	if (display_messages.size() > rows - 2)
	{
		first = display_messages.begin();
		last = first + (display_messages.size() - (rows - 2));
		display_messages.erase(first, last);
	}
	move_cursor_message();
	for (auto message : display_messages)
	{
		buffer += message.c_str();
		buffer += "\r\n";
	}
}

void Display::update_size()
{
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
	{
		cols = 30;
		rows = 10;
	}
	else
	{
		cols = ws.ws_col;
		rows = ws.ws_row;
	}
}

void Display::cleanup()
{
	clear_screen();
	disablerawmode();
}

void Display::display_rows()
{
	buffer += "\x1b[H";
	for (int y = 0; y < rows - 1; y++)
		buffer += "~\r\n";
}

void Display::print_to_screen()
{
	write(STDOUT_FILENO, buffer.c_str(), buffer.size());
}

void Display::move_cursor_stdin()
{
	char cursor[10];

	sprintf(cursor, "\x1b[%d;1H", y);
	buffer += cursor;
}

void Display::move_cursor_message()
{
	char cursor[10];

	sprintf(cursor, "\x1b[H");
	buffer += cursor;
}
