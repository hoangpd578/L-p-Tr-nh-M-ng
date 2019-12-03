// ChatClient.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ChatClient.h"

#include "stdio.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "winsock2.h"

#define WM_SOCKET WM_USER+1

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

SOCKET client;

BOOL isRegistered;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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

	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(9000);

	connect(client, (SOCKADDR *)&addr, sizeof(addr));

	isRegistered = FALSE;

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATCLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATCLIENT));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATCLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATCLIENT);
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
      CW_USEDEFAULT, 0, 550, 470, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL, 
	   10, 10, 350, 350, hWnd, (HMENU)IDC_LIST_MESSAGE, GetModuleHandle(NULL), NULL);
   
   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("LISTBOX"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOVSCROLL,
	   370, 10, 150, 400, hWnd, (HMENU)IDC_LIST_CLIENT, GetModuleHandle(NULL), NULL);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT(""), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 
	   10, 360, 250, 40, hWnd, (HMENU)IDC_EDIT_MESSAGE, GetModuleHandle(NULL), NULL);

   CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("BUTTON"), TEXT("CONNECT"), WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
	   270, 360, 90, 40, hWnd, (HMENU)IDC_BUTTON_SEND, GetModuleHandle(NULL), NULL);

   WSAAsyncSelect(client, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);

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
		if (WSAGETSELECTERROR(lParam)) {
			// Xu ly loi ket noi
			isRegistered = FALSE;
			SetDlgItemTextA(hWnd, IDC_BUTTON_SEND, "CONNECT");
			SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_RESETCONTENT, 0, 0);

			return 0;
		}

		if (WSAGETSELECTEVENT(lParam) == FD_READ)
		{
			char buf[256];
			int ret = recv(wParam, buf, sizeof(buf), 0);

			buf[ret] = 0;

			SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)buf);
			SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, WM_VSCROLL, SB_BOTTOM, 0);

			if (strncmp(buf, "[ok]", 4) == 0)
			{
				// Xu ly dang nhap thanh cong
				isRegistered = TRUE;
				SetDlgItemTextA(hWnd, IDC_BUTTON_SEND, "SEND");

				// Gui lenh list de lay danh sach id dang nhap
				send(client, "list", 4, 0);
			}
			else if (strncmp(buf, "[list]", 6) == 0)
			{
				// Xu ly ket qua lenh list
				char *pch = strtok(buf + 7, " \n");
				while (pch != NULL)
				{
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)pch);
					pch = strtok(NULL, " \n");
				}
			}
			else if (strncmp(buf, "[connect]", 9) == 0)
			{
				char clientId[32];
				sscanf(buf + 10, "%s", clientId);
				SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_ADDSTRING, 0, (LPARAM)clientId);
			}
			else if (strncmp(buf, "[disconnect]", 12) == 0)
			{
				char clientId[32], selectId[32];
				sscanf(buf + 13, "%s", clientId);

				int i = 0;
				int num = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETCOUNT, 0, 0);
				for (; i < num; i++)
				{
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETTEXT, i, (LPARAM)selectId);
					if (strcmp(selectId, clientId) == 0)
						break;
				}

				if (i < num)
					SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_DELETESTRING, i, 0);
			}
		}

		if (WSAGETSELECTEVENT(lParam) == FD_CLOSE)
		{
			// Xu ly dong ket noi
			isRegistered = FALSE;
			SetDlgItemTextA(hWnd, IDC_BUTTON_SEND, "CONNECT");
			SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_RESETCONTENT, 0, 0);

			return 0;
		}
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_BUTTON_SEND:
			{
				if (!isRegistered)
				{
					// Chua dang nhap
					char bufId[64];
					GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, bufId, sizeof(bufId));

					char sendBuf[256];
					sprintf(sendBuf, "client_id: %s\n", bufId);
					send(client, sendBuf, strlen(sendBuf), 0);
					SetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, "");
				}
				else
				{
					// Da danh nhap
					int i = SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETCURSEL, 0, 0);
					if (i >= 0)
					{
						char targetId[32];
						SendDlgItemMessageA(hWnd, IDC_LIST_CLIENT, LB_GETTEXT, i, (LPARAM)targetId);

						char msg[64];
						GetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, msg, sizeof(msg));

						char sendBuf[256];
						sprintf(sendBuf, "%s %s\n", targetId, msg);
						send(client, sendBuf, strlen(sendBuf), 0);

						SendDlgItemMessageA(hWnd, IDC_LIST_MESSAGE, LB_ADDSTRING, 0, (LPARAM)sendBuf);

						SetDlgItemTextA(hWnd, IDC_EDIT_MESSAGE, "");
					}
				}
			}
			break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
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
