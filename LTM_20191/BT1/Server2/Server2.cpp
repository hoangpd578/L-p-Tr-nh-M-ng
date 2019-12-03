// Server2.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

	SOCKET client = accept(listener, NULL, NULL);

	char buf[256];
	
	int ret = recv(client, buf, sizeof(buf), 0);

	// Xu ly du lieu
	buf[ret] = 0;
	
	int offset = 0;
	int n = buf[offset];
	offset++;
	for (int i = 0; i < n; i++)
	{
		char driveLetter = buf[offset];
		offset++;

		float driveSize;
		memcpy(&driveSize, buf + offset, 4);
		offset += 4;

		printf("%c:\\ - %fGB\n", driveLetter, driveSize);
	}

	printf("Computer name: %s\n", buf + offset);

	system("PAUSE");

	closesocket(client);
	closesocket(listener);
	WSACleanup();
}