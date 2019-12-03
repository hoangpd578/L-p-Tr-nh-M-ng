// Client1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	fd_set fdread;
	int ret;
	char buf[256];

	while (1)
	{
		// Clear tap fdread
		FD_ZERO(&fdread);

		// Them socket vao fdread
		FD_SET(client, &fdread);

		// Tham do su kien
		ret = select(0, &fdread, NULL, NULL, NULL);
		if (ret == SOCKET_ERROR)
			break;

		if (FD_ISSET(client, &fdread))
		{
			// Xu ly su kien cua client
			ret = recv(client, buf, sizeof(buf), 0);

			if (ret < 0)
				continue;

			buf[ret] = 0;
			printf("Received: %s", buf);
		}
	}
}