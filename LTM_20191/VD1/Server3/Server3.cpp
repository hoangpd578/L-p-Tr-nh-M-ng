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

	printf("Dang cho ket noi...\n");
	SOCKET client = accept(listener, NULL, NULL);

	printf("Ket noi thanh cong...\n");
	printf("Dang cho du lieu...\n");

	char buf[1024];
	int ret;

	//while (true)
	//{
	//	ret = recv(client, buf, sizeof(buf), 0);
	//	if (ret > 0)
	//	{
	//		/*if (ret < sizeof(buf))
	//			buf[ret] = 0;
	//		printf("Nhan duoc %d bytes: %s\n", ret, buf);*/

	//		// Nhan du lieu la so nguyen
	//		/*float f;
	//		memcpy(&f, buf, sizeof(f));

	//		printf("So nhan duoc: %f\n", f);*/

	//	}
	//	else
	//		break;
	//}

	// Nhan du lieu va luu vao file

	FILE *f = fopen("C:\\Test\\test.mp4", "wb");

	while (1)
	{
		ret = recv(client, buf, sizeof(buf), 0);
		if (ret <= 0)
			break;

		fwrite(buf, 1, ret, f);
	}

	fclose(f);

	closesocket(client);
	closesocket(listener);
	WSACleanup();
}