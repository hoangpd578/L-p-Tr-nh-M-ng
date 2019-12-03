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

	WSAEVENT newEvent = WSACreateEvent();
	WSAEventSelect(client, newEvent, FD_READ | FD_CLOSE);

	int ret;
	char buf[1024];

	while (true)
	{
		ret = WSAWaitForMultipleEvents(1, &newEvent, FALSE, 5000, FALSE);
		if (ret == WSA_WAIT_FAILED)
		{
			break;
		}

		if (ret == WSA_WAIT_TIMEOUT)
		{
			printf("Timed out\n");
			continue;
		}

		WSANETWORKEVENTS networkEvents;
		WSAEnumNetworkEvents(client, newEvent, &networkEvents);

		if (networkEvents.lNetworkEvents & FD_READ)
		{
			if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
			{
				continue;
			}

			ret = recv(client, buf, sizeof(buf), 0);
			buf[ret] = 0;
			printf("Received: %s\n", buf);
		}

		if (networkEvents.lNetworkEvents & FD_CLOSE)
		{
			if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
			{
				break;
			}
		}
	}
}