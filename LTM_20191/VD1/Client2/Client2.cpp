// Client2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"
#include "ws2tcpip.h"
int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;

	// Phan giai ten mien ra cau truc dia chi
	addrinfo *result;
	int ret = getaddrinfo("genk.vn", "http", NULL, &result);

	if (ret != 0)
	{
		printf("Loi phan giai ten mien");
		WSACleanup();
		return -1;
	}
	
	memcpy(&addr, result->ai_addr, result->ai_addrlen);
	printf("Dia chi IP: %s\n", inet_ntoa(addr.sin_addr));
		
	// Ket noi den server
	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	char cmd[] = "GET / HTTP/1.1\nHost: genk.vn\n\n";
	send(client, cmd, strlen(cmd), 0);

	char buf[1024];
	
	while (1)
	{
		// Nhan du lieu tu server
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		// Them ky tu xuong dong
		if (ret < sizeof(buf))
			buf[ret] = 0;

		printf(buf);
	}

	closesocket(client);
	WSACleanup();
}