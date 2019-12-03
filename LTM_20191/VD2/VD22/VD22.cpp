// VD22.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#include "winsock2.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	unsigned long ul = 1;
	ioctlsocket(listener, FIONBIO, &ul);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	SOCKET clients[64];
	int numClients = 0;

	char buf[256];
	int ret;

	while (1)
	{
		// printf("Waiting for client...\n");
		SOCKET client = accept(listener, NULL, NULL);
		if (client == INVALID_SOCKET)
		{
			ret = WSAGetLastError();
			if (ret == WSAEWOULDBLOCK)
			{
				// Loi do thao tac dang thuc hien
			} 
			else
			{
				// Xu ly loi khac
			}
		}
		else
		{
			printf("Client accepted: %d\n", client);
			// Them socket vao mang
			clients[numClients] = client;
			numClients++;
		}
		
		// Doc du lieu tu cac client
		for (int i = 0; i < numClients; i++)
		{
			// printf("Waiting for data...\n", client);
			ret = recv(clients[i], buf, sizeof(buf), 0);
			if (ret == SOCKET_ERROR)
			{
				// Bi loi
				ret = WSAGetLastError();
				if (ret == WSAEWOULDBLOCK)
				{
					// Loi do thao tac dang thuc hien
				}
				else
				{
					// Xu ly loi khac
				}
			}
			else if (ret == 0)
			{
				// Ket noi bi ngat
			}
			else
			{
				// Co du lieu, xu ly du lieu
				buf[ret] = 0;
				printf("Receive from %d: %s\n", clients[i], buf);
			}			
		}
	}
}