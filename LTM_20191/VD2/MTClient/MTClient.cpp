// MTClient.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"

DWORD WINAPI ReceiverThread(LPVOID);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);
    
	system("PAUSE");
	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	CreateThread(0, 0, ReceiverThread, &client, 0, 0);

	char buf[64];
	while (1)
	{
		printf("Nhap du lieu: ");
		gets_s(buf, sizeof(buf));

		if (strncmp(buf, "exit", 4) == 0)
			break;

		send(client, buf, strlen(buf), 0);
	}

	closesocket(client);
}

DWORD WINAPI ReceiverThread(LPVOID lpParam)
{
	char buf[256];
	int ret;

	SOCKET client = *(SOCKET *)lpParam;

	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		buf[ret] = 0;
		printf("Received: %s\n", buf);
	}
}