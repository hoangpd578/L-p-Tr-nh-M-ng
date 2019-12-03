// TelnetServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#include "winsock2.h"

DWORD WINAPI ClientThread(LPVOID);
void RemoveClient(SOCKET client);

SOCKET clients[64];
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
	char buf[256], fbuf[256], cmdbuf[256];
	int ret;

	char msg[] = "Sai cu phap. Hay gui lai.\n";

	// Vong lap hoi username va password
	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			return 1;

		buf[ret] = 0;
		printf("Received: %s", buf);

		// Doc du lieu tu file
		FILE *f = fopen("C:\\Test\\data.txt", "r");
		int found = 0;

		while (!feof(f))
		{
			fgets(fbuf, sizeof(fbuf), f);
			if (strcmp(buf, fbuf) == 0)
			{
				found = 1;
				break;
			}				
		}
		fclose(f);

		if (found == 1)
		{
			char msg[] = "Dang nhap thanh cong. Hay nhap lenh de thuc hien\n";
			send(client, msg, strlen(msg), 0);
			break;
		}
		else 
		{
			char msg[] = "Dang nhap that bai. Hay nhap lai username va password\n";
			send(client, msg, strlen(msg), 0);
		}
	}

	// Vong lap thuc hien lenh
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

		// Xoa ky tu xuong dong
		if (buf[ret - 1] == '\n')
			buf[ret - 1] = 0;

		// Tao xau thuc hien lenh
		sprintf(cmdbuf, "%s > c:\\test\\out.txt", buf);

		// Thuc hien lenh
		system(cmdbuf);

		// Doc ket qua va tra ve cho client
		FILE *f = fopen("C:\\test\\out.txt", "r");
		while (!feof(f))
		{
			fgets(fbuf, sizeof(fbuf), f);
			send(client, fbuf, strlen(fbuf), 0);
		}
		fclose(f);
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
			clients[i] = clients[numClients - 1];

		numClients--;
	}
}