// Client1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "stdio.h"

#define WM_SOCKET WM_USER + 1

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

bool CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	// Tao doi tuong cua so
	WNDCLASS wndclass;
	const CHAR *providerClass = "AsyncSelect";
	HWND window;

	wndclass.style = 0;
	wndclass.lpfnWndProc = (WNDPROC)WindowProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = NULL;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = (LPCWSTR)providerClass;

	if (RegisterClass(&wndclass) == 0)
		return NULL;

	// Create a window
	if ((window = CreateWindow((LPCWSTR)providerClass, L"", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, NULL, NULL, NULL)) == NULL)
		return NULL;

	WSAAsyncSelect(client, window, WM_SOCKET, FD_READ | FD_CLOSE);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

bool CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_SOCKET) // Ma thong diep cua socket
	{
		if (WSAGETSELECTERROR(lParam))
		{
			closesocket(wParam);
			return false;
		}

		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			char buf[256];
			int ret = recv(wParam, buf, sizeof(buf), 0);
			if (ret <= 0)
			{
				closesocket(wParam);
				return false;
			}

			buf[ret] = 0;
			printf("Received: %s", buf);
		}

		if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
		{
			closesocket(wParam);
			return false;
		}
	}

	return true;
}