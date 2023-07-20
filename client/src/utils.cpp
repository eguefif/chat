#include "client.h"

void signal_handler(int signal)
{
	if (signal == SIGINT)
		g_running = false;
	if (signal == SIGTERM)
		g_running = false;
}
