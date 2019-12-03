// UDPReceiver.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "stdio.h"
#include "winsock2.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(receiver, (SOCKADDR *)&addr, sizeof(addr));
	
	char buf[1024];

	while (1)
	{
		int ret = recvfrom(receiver, buf, sizeof(buf), 0, NULL, NULL);
		if (ret <= 0)
			break;

		buf[ret] = 0;
		printf("Received: %s\n", buf);
	}

	WSACleanup();
}
