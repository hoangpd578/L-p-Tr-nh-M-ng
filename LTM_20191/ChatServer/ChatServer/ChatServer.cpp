// ChatServer.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ChatServer.h"

#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

#define MAX_LOADSTRING 100

#define WM_SOCKET WM_USER+1

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

SOCKET listener;
int numRegisteredClients;
SOCKET registeredClients[64];
char * registeredIds[64];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void RemoveClient(HWND, SOCKET);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(9000);

	bind(listener, (SOCKADDR *)&addr, sizeof(addr));
	listen(listener, 5);

	numRegisteredClients = 0;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATSERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATSERVER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATSERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATSERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 450, 420, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 
	   10, 10, 150, 300, hWnd, (HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
	   170, 10, 250, 350, hWnd, (HMENU)IDC_LIST_MESSAGE, GetModuleHandle(NULL), NULL);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("CLOSE"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
	   10, 320, 150, 30, hWnd, (HMENU)IDC_BUTTON_CLOSE, GetModuleHandle(NULL), NULL);

   WSAAsyncSelect(listener, hWnd, WM_SOCKET, FD_ACCEPT);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_SOCKET:
	{
		if (WSAGETSELECTERROR(lParam))
		{
			// Xu ly loi bi huy
			if (wParam != listener) 
			{
				RemoveClient(hWnd, wParam);
			}
			return 0;
		}

		if (WSAGETSELECTEVENT(lParam) == FD_ACCEPT)
		{
			SOCKET client = accept(wParam, NULL, NULL);
			SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)"New client accepted");

			WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
		}

		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			char buf[256];
			int ret = recv(wParam, buf, sizeof(buf), 0);
			
			// Them ky tu ket thuc xau
			buf[ret] = 0;

			// Them vao list message
			SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);
			SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, WM_VSCROLL, SB_BOTTOM, 0);

			// Xu ly du lieu
			int j = 0;
			for (; j < numRegisteredClients; j++)
				if (wParam == registeredClients[j])
					break;

			if (j == numRegisteredClients) // Chua dang nhap
			{
				char cmd[16], id[32], tmp[32];
				char msg[] = "[error] Sai cu phap. Hay nhap lai.\n";

				// Xu ly du lieu nhan duoc
				ret = sscanf(buf, "%s %s %s", cmd, id, tmp);
				if (ret != 2)
				{
					// Sai cu phap
					send(wParam, msg, strlen(msg), 0);
				}
				else
				{
					if (strcmp(cmd, "client_id:") != 0)
					{
						// Sai cu phap
						send(wParam, msg, strlen(msg), 0);
					}
					else
					{
						// Kiem tra trung lap id
						j = 0;
						for (; j < numRegisteredClients; j++)
							if (strcmp(registeredIds[j], id) == 0)
								break;
						if (j < numRegisteredClients)
						{
							// Bi trung id
							char errMsg[] = "[error] Bi trung lap id. Hay nhap lai.\n";
							send(wParam, errMsg, strlen(errMsg), 0);
						}
						else
						{
							// Dang nhap thanh cong
							char okMsg[] = "[ok] Dang nhap thanh cong. Gui message de forward.\n";
							send(wParam, okMsg, strlen(okMsg), 0);

							// Gui thong bao cho cac client hien tai
							char sendBuf[64];
							sprintf(sendBuf, "[connect] %s\n", id);
							for (j = 0; j < numRegisteredClients; j++)
								send(registeredClients[j], sendBuf, strlen(sendBuf), 0);

							// Them socket vao mang
							registeredClients[numRegisteredClients] = wParam;
							registeredIds[numRegisteredClients] = (char *)malloc(strlen(id) + 1);
							memcpy(registeredIds[numRegisteredClients], id, strlen(id) + 1);
							numRegisteredClients++;

							// Them vao list client
							SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)id);
							SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, WM_VSCROLL, SB_BOTTOM, 0);
						}						
					}
				}
			}
			else // Da dang nhap
			{
				char target_id[16], tmp[32];
				char msg[] = "[error] Sai cu phap. Hay nhap lai";

				ret = sscanf(buf, "%s %s", target_id, tmp);
				if (ret < 2)
				{
					if (strcmp(target_id, "list") == 0)
					{
						char sendbuf[256];
						strcpy(sendbuf, "[list] ");

						// Gui danh sach client
						for (int j = 0; j < numRegisteredClients; j++) 
						{
							strcat(sendbuf, registeredIds[j]);
							if (j < numRegisteredClients - 1)
								strcat(sendbuf, " ");
							else
								strcat(sendbuf, "\n");
						}
						send(wParam, sendbuf, strlen(sendbuf), 0);
					}
					else if (strcmp(target_id, "disconnect") == 0)
					{
						RemoveClient(hWnd, wParam);
					}
					else 
						send(wParam, msg, strlen(msg), 0);
				}
				else
				{
					char sendbuf[256];
					sprintf(sendbuf, "%s: %s", registeredIds[j], buf + strlen(target_id) + 1);

					if (strcmp(target_id, "all") == 0)
					{
						// Forward message den cac client
						for (int j = 0; j < numRegisteredClients; j++)
							if (wParam != registeredClients[j])
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
							char msg[] = "[error] Khong tim thay id de gui thong diep.\n";
							send(wParam, msg, strlen(msg), 0);
						}
					}
				}
			}
		}

		if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
		{
			// Xu ly ket noi bi dong
			RemoveClient(hWnd, wParam);
		}
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case IDC_BUTTON_CLOSE:
			{
				int i = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETCURSEL, 0, 0);
				if (i >= 0 && i < numRegisteredClients)
					RemoveClient(hWnd, registeredClients[i]);
			}
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void RemoveClient(HWND hWnd, SOCKET client)
{
	int i = 0;
	for (; i < numRegisteredClients; i++)
		if (client == registeredClients[i])
			break;

	if (i < numRegisteredClients) 
	{
		char sendBuf[64];
		sprintf(sendBuf, "[disconnect] %s\n", registeredIds[i]);

		// Dong socket
		closesocket(client);

		// Xoa khoi mang
		for (int j = i; j < numRegisteredClients - 1; j++)
		{
			registeredClients[j] = registeredClients[j + 1];
			registeredIds[j] = registeredIds[j + 1];
		}

		numRegisteredClients--;

		// Xoa khoi list box
		SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_DELETESTRING, i, 0);

		// Gui thong bao cho cac client hien tai
		for (int j = 0; j < numRegisteredClients; j++)
			send(registeredClients[j], sendBuf, strlen(sendBuf), 0);
	}
}