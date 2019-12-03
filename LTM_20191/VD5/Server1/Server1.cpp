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

	SOCKET sockets[64];
	WSAEVENT events[64];
	int numEvents = 0;

	WSAEVENT newEvent = WSACreateEvent();
	WSAEventSelect(listener, newEvent, FD_ACCEPT);

	sockets[numEvents] = listener;
	events[numEvents] = newEvent;
	numEvents++;

	while (1)
	{
		int ret = WSAWaitForMultipleEvents(numEvents, events, FALSE, WSA_INFINITE, FALSE);
		if (ret == WSA_WAIT_FAILED)
		{
			continue;
		}

		int index = ret - WSA_WAIT_EVENT_0;
		for (int i = index; i < numEvents; i++)
		{
			ret = WSAWaitForMultipleEvents(1, &events[i], FALSE, 0, FALSE);
			if (ret == WSA_WAIT_FAILED || ret == WSA_WAIT_TIMEOUT)
				continue;

			WSANETWORKEVENTS networkEvents;
			WSAEnumNetworkEvents(sockets[i], events[i], &networkEvents);

			if (networkEvents.lNetworkEvents & FD_ACCEPT)
			{
				if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
					continue;

				SOCKET client = accept(sockets[i], NULL, NULL);

				if (numEvents >= 64)
				{
					printf("Da vuot qua so luong socket toi da");
					closesocket(client);
					continue;
				}

				newEvent = WSACreateEvent();
				WSAEventSelect(client, newEvent, FD_READ | FD_CLOSE);

				sockets[numEvents] = client;
				events[numEvents] = newEvent;
				numEvents++;

				printf("New client accepted: %d\n", client);
			}

			if (networkEvents.lNetworkEvents & FD_READ)
			{
				if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
				{
					continue;
				}

				char buf[256];
				ret = recv(sockets[i], buf, sizeof(buf), 0);
				buf[ret] = 0;
				printf("Received: %s\n", buf);
			}

			if (networkEvents.lNetworkEvents & FD_CLOSE)
			{
				if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
				{
					continue;
				}
			}
		}

	}
}