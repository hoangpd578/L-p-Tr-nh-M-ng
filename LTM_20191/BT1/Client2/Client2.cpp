// Client2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "winsock2.h"

int main()
{
	char computerName[64];
	DWORD nameLen;

	GetComputerNameA(computerName, &nameLen);

	char driveLetters[64];
	int ret = GetLogicalDriveStringsA(sizeof(driveLetters), driveLetters);

	char buf[64];
	int offset = 0;

	// Tao buffer chua du lieu gui sang server
	// Byte dau tien la so o dia
	buf[offset] = ret / 4;
	offset++;

	// Cac byte tiep theo luu nhan va kich thuoc o dia
	for (int i = 0; i < ret / 4; i++)
	{
		// Nhan o dia
		char driveLetter = driveLetters[i * 4];
		buf[offset] = driveLetter;
		offset++;

		// Kick thuoc
		DWORD sectorsPerCluster, bytesPerSector, numberOfFreeClusters, totalNumberOfClusters;
		GetDiskFreeSpaceA(&driveLetters[i * 4], &sectorsPerCluster, &bytesPerSector, &numberOfFreeClusters, &totalNumberOfClusters);

		float diskSize = (float)totalNumberOfClusters * (float)sectorsPerCluster * (float)bytesPerSector / (1 << 30);
		memcpy(buf + offset, &diskSize, 4);
		offset += 4;
	}

	// Luu ten may tinh
	memcpy(buf + offset, computerName, nameLen);
	offset += nameLen;


	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	system("PAUSE");
	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	send(client, buf, offset, 0);
	closesocket(client);
	WSACleanup();
}
