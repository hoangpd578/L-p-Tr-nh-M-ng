// Server1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#include "winsock2.h"

int main(int argc, char *argv[])
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(atoi(argv[1]));

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET client = accept(listener, NULL, NULL);
	
	char buf[256];
	int ret;

	FILE *f1 = fopen(argv[2], "r");
	fgets(buf, sizeof(buf), f1);
	fclose(f1);

	send(client, buf, strlen(buf), 0);

	FILE *f2 = fopen(argv[3], "a");
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		buf[ret] = '\n';
		buf[ret + 1] = 0;

		printf("Recieved: %s\n", buf);

		fputs(buf, f2);
	}

	fclose(f2);

	closesocket(client);
	closesocket(listener);

	WSACleanup();
}