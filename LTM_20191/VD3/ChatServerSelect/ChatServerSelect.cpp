// ChatServerSelect.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

	fd_set fdread;
	int ret;

	// Luu client ket noi den server
	SOCKET clients[64];
	int numClients = 0;

	// Luu client da dang nhap vao server
	SOCKET registeredClients[64];
	char* registeredIds[64];
	int numRegisteredClients = 0;

	char buf[256];

	struct timeval tv;
	tv.tv_usec = 0;
	tv.tv_sec = 5;

	char cmd[16], id[64], tmp[64], target_id[64];
	char msg[] = "Sai cu phap. Hay gui lai.\n";

	while (1)
	{
		FD_ZERO(&fdread);

		FD_SET(listener, &fdread);
		for (int i = 0; i < numClients; i++)
			FD_SET(clients[i], &fdread);

		ret = select(0, &fdread, NULL, NULL, NULL);

		if (ret < 0)
			break;

		if (ret == 0)
		{
			printf("Timed out...\n");
		}

		if (FD_ISSET(listener, &fdread))
		{
			// Xu ly su kien chap nhan ket noi
			SOCKET client = accept(listener, NULL, NULL);

			printf("New client accepted: %d\n", client);

			// Them socket vao mang
			clients[numClients] = client;
			numClients++;
		}

		for (int i = 0; i < numClients; i++)
			if (FD_ISSET(clients[i], &fdread))
			{
				// Xu ly su kien nhan du lieu
				ret = recv(clients[i], buf, sizeof(buf), 0);
				if (ret < 0)
					continue;

				// Xu ly du lieu
				buf[ret] = 0;
				printf("Received from %d: %s", clients[i], buf);

				int j = 0;
				for (; j < numRegisteredClients; j++)
					if (clients[i] == registeredClients[j])
						break;

				if (j == numRegisteredClients) // Chua dang nhap
				{
					// Xu ly du lieu nhan duoc
					ret = sscanf(buf, "%s %s %s", cmd, id, tmp);
					if (ret != 2)
					{
						// Sai cu phap
						send(clients[i], msg, strlen(msg), 0);
					}
					else
					{
						if (strcmp(cmd, "client_id:") != 0)
						{
							// Sai cu phap
							send(clients[i], msg, strlen(msg), 0);
						}
						else
						{
							// Dung cu phap
							char okmsg[] = "Dung cu phap. Gui message de forward.\n";
							send(clients[i], okmsg, strlen(okmsg), 0);

							// Them socket vao mang
							registeredClients[numRegisteredClients] = clients[i];
							registeredIds[numRegisteredClients] = (char *)malloc(strlen(id) + 1);
							memcpy(registeredIds[numRegisteredClients], id, strlen(id) + 1);
							numRegisteredClients++;
						}
					}
				}
				else // Da dang nhap
				{
					ret = sscanf(buf, "%s %s", target_id, tmp);
					if (ret < 2)
					{
						send(clients[i], msg, strlen(msg), 0);
					}
					else
					{
						char sendbuf[256];
						sprintf(sendbuf, "%s: %s", id, buf + strlen(target_id) + 1);

						if (strcmp(target_id, "all") == 0)
						{
							// Forward message den cac client
							for (int j = 0; j < numRegisteredClients; j++)
								if (clients[i] != registeredClients[j])
									send(registeredClients[j], sendbuf, strlen(sendbuf), 0);
						}
						else
						{
							int j = 0;
							for (; j < numRegisteredClients; j++)
								if (strcmp(target_id, registeredIds[j]) == 0)
									break;

							if (j < numRegisteredClients)
							{
								send(registeredClients[j], sendbuf, strlen(sendbuf), 0);
							}
							else
							{
								char msg[] = "Khong tim thay id de gui thong diep.\n";
								send(clients[i], msg, strlen(msg), 0);
							}
						}
					}
				}
			}

	}
}