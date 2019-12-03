// Server1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#include "winsock2.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	fd_set fdread;
	int ret;

	SOCKET clients[64];
	int numClients = 0;

	char buf[256];

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 5;

	while (1)
	{
		FD_ZERO(&fdread);
		
		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		ret = select(0, &fdread, NULL, NULL, &tv);

		if (ret < 0)
			break;

		if (ret == 0)
		{
			printf("Timed out...\n");
		}

		if (FD_ISSET(listener, &fdread))
		{
			// Xu ly su kien chap nhan ket noi
			SOCKET client = accept(listener, NULL, NULL);

			printf("New client accepted: %d\n", client);

			// Them socket vao mang
			clients[numClients] = client;
			numClients++;
		}

		for (int i = 0; i < numClients; i++)
			if (FD_ISSET(clients[i], &fdread))
			{
				// Xu ly su kien nhan du lieu
				ret = recv(clients[i], buf, sizeof(buf), 0);
				if (ret < 0)
					continue;

				// Xu ly du lieu
				buf[ret] = 0;
				printf("Received from %d: %s", clients[i], buf);
			}

	}
}