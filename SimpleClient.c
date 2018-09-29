#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 200
struct addrinfo *get_addrinfo(char *server , char *port)
{
	struct addrinfo hints, *res;
	int i, ret;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	
	for (i = 0; server[i] != '\0'; i++)
	{
		if (server[i] == '.')
		{
			i = -1;
			break;
		}
	}
	if (i == -1)
	{	
		char www_str[150] = "www.";
		strcat(www_str, server);
		ret = getaddrinfo(www_str, port, &hints, &res);
	}
	else
	{
		ret = getaddrinfo(server, port, &hints, &res);	
	}
	if (ret != 0)
	{
		printf("Error getting addrinfo %s\n", gai_strerror(ret));
		return NULL;
	}
	return res;
}
//need to free addrinfo array
int connect_to_server(struct addrinfo *addr)
{
	if (addr == NULL)
		return -1;
	int socketfd;
	for (; addr != NULL; addr = addr->ai_next)
	{
		printf("Addr object started\n");
		if (socketfd = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol) < 0)
		{
			printf("Error initialising socket %s \n", addr->ai_canonname);
			continue;
		}
		if (connect(socketfd, addr->ai_addr, addr->ai_addrlen) < 0)
		{
			close(socketfd);
			//printf("Error connecting %s\n", addr->ai_canonname);
			continue;
		}
		return socketfd;
	}
	return -1;
}

int send_get_request(int socketfd, char *server, char *index)
{
	char request [500] = {0};
	sprintf(request, "GET %s HTTP/1.1\r\nHost: www.%s\r\nConnection: close\r\n\r\n",  index, server);
	printf("%s\n", request );
	if (send(socketfd, request, strlen(request), 0) < 0)
		return -1;
	else
		return 0;
}
int main(int argc, char *argv[])
{
	char port_num[6] = "";
	char recv_buffer[BUFFER_SIZE] = {0};
	switch (argc)
	{
	case 1:
		printf("Enter URL\n");
		exit(1);
		break;
	case 2:
		strcpy(port_num , "80");
		break;
	case 3:
		strcpy(port_num, argv[2]);
		break;
	default:
		printf("Too many arguments!\n");
		exit(1);
	}

	int socketfd = connect_to_server(get_addrinfo(argv[1], port_num));
	if (socketfd < 0)
	{
		printf("Unable to connect to %s : %s \n", argv[1], port_num );
		close(socketfd);
		return 1;
	}

	if (send_get_request(socketfd, argv[1], "/") < 0)
	{
		close(socketfd);
		return 1;
	}
	while (recv(socketfd, recv_buffer, BUFFER_SIZE, 0) > 0)
	{
		printf("%s\n", recv_buffer);
		memset(recv_buffer, 0, BUFFER_SIZE);
	}
	close(socketfd);
	return 0;
}