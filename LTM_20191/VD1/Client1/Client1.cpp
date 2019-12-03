#include "pch.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "stdio.h"
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

	int ret = connect(client, (SOCKADDR *)&addr, sizeof(addr));

	if (ret == SOCKET_ERROR)
	{
		ret = WSAGetLastError();
		printf("Loi ket noi: %d\n", ret);
		return -1;
	}

	char msg[] = "Hello Server";
	send(client, msg, strlen(msg), 0);

	char buf[256];
	ret = recv(client, buf, 256, 0);

	buf[ret] = 0;
	printf("Received: %s", buf);

	closesocket(client);
	WSACleanup();
}