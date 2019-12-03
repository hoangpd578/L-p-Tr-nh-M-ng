// UDPSender.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	char msg[] = "Hello UDP!";
	sendto(sender, msg, strlen(msg), 0, (SOCKADDR *)&addr, sizeof(addr));

	closesocket(sender);
	WSACleanup();
}

