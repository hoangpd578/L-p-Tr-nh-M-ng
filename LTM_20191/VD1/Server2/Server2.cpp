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

	char buf[1024];
	int ret;

	char cmd[16], path[256], temp[1024];

	while (1)
	{
		// Chap nhan ket noi
		SOCKET client = accept(listener, NULL, NULL);
		
		// Nhan du lieu
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			continue;

		if (ret < sizeof(buf))
			buf[ret] = 0;
		printf("Received: %s\n", buf);

		ret = sscanf(buf, "%s %s %s", cmd, path, temp);

		// Xu ly yeu cau nhan duoc
		if (strcmp(path, "/vi") == 0)
		{
			char msg[] = "HTTP/1.1 200 OK\nContent-Type: text/text\n\n<html><body><h1>Xin chao cac ban</h1></body></html>";
			send(client, msg, strlen(msg), 0);
		}
		else if (strcmp(path, "/en") == 0)
		{
			char msg[] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Hello World</h1></body></html>";
			send(client, msg, strlen(msg), 0);
		}
		else if (strcmp(path, "/image") == 0)
		{
			char msg[] = "HTTP/1.1 200 OK\nContent-Type: image/jpeg\n\n";
			send(client, msg, strlen(msg), 0);

			FILE *f = fopen("C:\\Test\\car3.jpg", "rb");
			char fbuf[1024];
			while (true)
			{
				ret = fread(fbuf, 1, 1024, f);
				if (ret <= 0)
					break;
				send(client, fbuf, ret, 0);
			}

			fclose(f);
		}
		else
		{
			char msg[] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body>Lenh khong duoc ho tro</body></html>";
			send(client, msg, strlen(msg), 0);
		}

		closesocket(client);
	}


}