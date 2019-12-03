// CharServerMT.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#include "winsock2.h"

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET client);

SOCKET clients[64];
char* ids[64];
int numClients;

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

	numClients = 0;

	while (1)
	{
		SOCKET client = accept(listener, NULL, NULL);
		printf("New client accepted: %d\n", client);
		CreateThread(0, 0, ClientThread, &client, 0, 0);
	}
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET client = *(SOCKET *)lpParam;
	char buf[256];
	int ret;

	char cmd[16], id[64], tmp[64], target_id[64];

	char msg[] = "Sai cu phap. Hay gui lai.\n";

	// Vong lap hoi client_id
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			return 1;

		buf[ret] = 0;
		printf("Received: %s", buf);

		// Xu ly du lieu nhan duoc
		ret = sscanf(buf, "%s %s %s", cmd, id, tmp);
		if (ret != 2)
		{
			// Sai cu phap
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			if (strcmp(cmd, "client_id:") != 0)
			{
				// Sai cu phap
				send(client, msg, strlen(msg), 0);
			}
			else
			{
				// Dung cu phap
				char okmsg[] = "Dung cu phap. Gui message de forward.\n";
				send(client, okmsg, strlen(okmsg), 0);

				// Them socket vao mang
				clients[numClients] = client;
				ids[numClients] = id;
				numClients++;
				break;
			}
		}
	}

	// Vong lap forward message
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
		{
			RemoveClient(client);
			return 1;
		}
			

		buf[ret] = 0;
		printf("Received: %s", buf);

		ret = sscanf(buf, "%s %s", target_id, tmp);
		if (ret < 2) 
		{
			send(client, msg, strlen(msg), 0);
		}
		else
		{
			char sendbuf[256];
			sprintf(sendbuf, "%s: %s", id, buf + strlen(target_id) + 1);

			if (strcmp(target_id, "all") == 0)
			{
				// Forward message den cac client
				for (int i = 0; i < numClients; i++)
					if (clients[i] != client)
						send(clients[i], sendbuf, strlen(sendbuf), 0);
			}
			else 
			{
				int i = 0;
				for (; i < numClients; i++)
					if (strcmp(target_id, ids[i]) == 0)
						break;

				if (i < numClients)
				{
					send(clients[i], sendbuf, strlen(sendbuf), 0);
				}
				else
				{
					char msg[] = "Khong tim thay id de gui thong diep.\n";
					send(client, msg, strlen(msg), 0);
				}
			}
		}
	}
}

void RemoveClient(SOCKET client)
{
	// Tim vi tri can xoa
	int i = 0;
	for (; i < numClients; i++)
		if (clients[i] == client)
			break;

	// Xoa phan tu tai vi tri i
	if (i < numClients)
	{
		if (i < numClients - 1)
		{
			clients[i] = clients[numClients - 1];
			ids[i] = ids[numClients - 1];
		}

		numClients--;
	}
}