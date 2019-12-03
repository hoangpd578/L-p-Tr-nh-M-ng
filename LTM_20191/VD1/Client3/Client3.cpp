// Client3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

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
	
	// Ket noi den server
	system("PAUSE");
	connect(client, (SOCKADDR *)&addr, sizeof(addr));
	
	int ret;
	char buf[1024];
	
	// float f;

	//while (1)
	//{
	//	// Doc chuoi ky tu tu ban phim
	//	/*printf("Nhap du lieu: ");
	//	gets_s(buf, sizeof(buf));

	//	send(client, buf, strlen(buf), 0);*/

	//	// Doc so tu ban phim
	//	printf("Nhap so thuc: ");
	//	scanf("%f", &f);

	//	send(client, (char *)&f, sizeof(f), 0);
	//}

	FILE *f = fopen("C:\\Test_client\\ecard.mp4", "rb");
	while (1)
	{
		ret = fread(buf, 1, sizeof(buf), f);
		if (ret <= 0)
			break;

		send(client, buf, ret, 0);
	}

	fclose(f);

	closesocket(client);
	WSACleanup();
}