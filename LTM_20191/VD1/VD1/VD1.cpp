#include "pch.h"

#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"
#include "ws2tcpip.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
	
	addrinfo *result;

	int ret = getaddrinfo("hust.edu.vn", "http", NULL, &result);

	if (ret == 0)
	{
		SOCKADDR_IN addr;
		memcpy(&addr, result->ai_addr, result->ai_addrlen);

		printf("Dia chi IP: %s", inet_ntoa(addr.sin_addr));
	}
	else
		printf("Loi phan giai ten mien");

	WSACleanup();
	return 0;
}
