#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 500

void run(char address[], int port);

int main(int argc, char *argv[])
{
	char address[] = "127.0.0.1";
	int port = 10000;
	if (argc > 1)
	{
		strncpy(address, argv[1], 10);
		port = atoi(argv[2]);
	}

	run(address, port);
	return (0);
}

void trim(char str[])
{
	int c = 0;

	while (str[c] != '\0')
	{
		if (str[c] == '\n')
			str[c] = '\0';
		c++;
	}
}

void run(char address[], int port)
{
	int socketClient;
	struct sockaddr_in addressClient;
	char message[MAX] = "";

	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	addressClient.sin_family = AF_INET;
	addressClient.sin_port = htons(port);
	addressClient.sin_addr.s_addr = inet_addr(address);
	memset(addressClient.sin_zero, '\0', sizeof(addressClient.sin_zero));
	connect(socketClient, (struct sockaddr *) &addressClient, sizeof(addressClient));

	memset(message, 0, sizeof(message));
	strcpy(message, "")
	send(socketClient, message, strlen(message), 0);
	close(socketClient);
}
