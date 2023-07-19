#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX 500

void process_message(int sock);
void run(char address[], int port);
void send_message(char std[], int socketclient);
void process_message2(int sock);

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

void run(char address[], int port)
{
	int socketClient;
	int sock2;
	struct sockaddr_in addressClient;

	socketClient = socket(AF_INET, SOCK_STREAM, 0);
	addressClient.sin_family = AF_INET;
	addressClient.sin_port = htons(port);
	addressClient.sin_addr.s_addr = inet_addr(address);
	memset(addressClient.sin_zero, '\0', sizeof(addressClient.sin_zero));
	connect(socketClient, (struct sockaddr *) &addressClient, sizeof(addressClient));

	sock2 = socket(AF_INET, SOCK_STREAM, 0);
	addressClient.sin_family = AF_INET;
	addressClient.sin_port = htons(port);
	addressClient.sin_addr.s_addr = inet_addr(address);
	memset(addressClient.sin_zero, '\0', sizeof(addressClient.sin_zero));
	connect(sock2, (struct sockaddr *) &addressClient, sizeof(addressClient));

	printf("\nTest name\n");
	send_message("00012nameAlfred", sock2);
	send_message("00012nameEmmanuel", socketClient);
	printf("\nTest list\n");
	send_message("00012nameAlfred", sock2);
	send_message("00004list", sock2);
	process_message(sock2);
	printf("Test chat\n");
	send_message("00036chatAlfred              Salut Alfred", socketClient);
	process_message2(sock2);
	printf("\nTest end of connexion\n");
	send_message("00003EOC", socketClient);
	send_message("00003EOC", sock2);

	close(socketClient);
	close(sock2);
}

void send_message(char str[], int socketclient)
{
	char message[MAX] = "";

	memset(message, '\0', MAX);
	strcpy(message, str);
	send(socketclient, message, strlen(message), 0);
}

void process_message(int sock)
{
	char protoheader[10];
	char content[500];
	int size;

	printf("Reading list return");
	read(sock, protoheader, 5);
	size = atoi(protoheader);
	printf( "size %d\n", size);
	read(sock, content, size);
	printf("List of user: \n");
	for (size_t i=4; i < strlen(content); i++)
	{
		if (content[i] == '\0')
			break;
		else if(content[i] == ' ')
			printf("\n");
		printf("%c", content[i]);
	}
	printf("\n");
}

void process_message2(int sock)
{
	char protoheader[10];
	char content[500];
	int size;

	printf("Reading answer to chat:\n");
	read(sock, protoheader, 5);
	size = atoi(protoheader);
	printf( "size %d\n", size);
	read(sock, content, size);
	for (size_t i = 4; i < 20; i++)
	{
		if (content[i] == ' ')
				break;
		printf("%c", content[i]);
	}
	printf(": ");
	for (size_t i=24; i < strlen(content); i++)
	{
		if (content[i] == '\0')
			break;
		printf("%c", content[i]);
	}
	printf("\n");
}
