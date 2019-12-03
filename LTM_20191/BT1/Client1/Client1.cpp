// Client1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"

int main(int argc, char* argv[])
{
	// argv[1] dia chi IP
	// argv[2] cong

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(argv[1]);
	addr.sin_port = htons(atoi(argv[2]));

	system("PAUSE");
	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	char buf[64];
	int ret = recv(client, buf, sizeof(buf), 0);
	
	buf[ret] = 0;
	printf("Received: %s\n", buf);

	while (1)
	{
		printf("Nhap du lieu can gui: ");
		gets_s(buf, sizeof(buf));

		send(client, buf, strlen(buf), 0);

		if (strncmp(buf, "exit", 4) == 0)
			break;
	}

	closesocket(client);
	WSACleanup();
}