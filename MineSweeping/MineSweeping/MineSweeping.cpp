// MineSweeping.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MineSweeping.h"
#include "Game.h"
#include <sstream>

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

//自定义常量
const int g_board_x = 10;
const int g_board_y = 60;
const int g_side_grid = 20;
const int TIMER = 1;

//自定义全局变量
int g_nTimer;
wchar_t g_cTimer[10];
wchar_t g_cRest[10];
int g_nCursor_x, g_nCursor_y;
int g_nClient_x, g_nClient_y;
LEVEL g_level = PRIMARY;
Game *g_game;
HWND g_hStatic_timer, g_hStatic_rest, g_hEdit_timer, g_hEdit_rest;

//函数声明
void AlignWindow(HWND hwnd, int x = 9, int y = 9);
void DrawBK(HDC hdc);
void InitGame(HWND hwnd, LEVEL l);
void OverGame(HWND hwnd);
void MouseLeft(HWND hwnd);
void MouseRight(HWND hwnd);
void DrawNum(HDC hdc, const int x, const int y);
void DrawMine(HDC hdc, const int x, const int y);
void DrawFlag(HDC hdc, const int x, const int y);
void DrawClient(HDC hdc);

// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MINESWEEPING, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINESWEEPING));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEEPING));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_MINESWEEPING);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc, memDc;
	HBITMAP bmp;

	switch (message)
	{
	case WM_CREATE:
		AlignWindow(hWnd);
		g_hStatic_timer = CreateWindow(L"STATIC", L"Timer", WS_CHILD | WS_VISIBLE | SS_CENTER,
			g_board_x, g_board_y - 50, 50, 15, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hStatic_rest = CreateWindow(L"STATIC", L"Step", WS_CHILD | WS_VISIBLE | SS_CENTER,
			g_board_x + 60, g_board_y - 50, 50, 15, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hEdit_timer = CreateWindow(L"EDIT", L"timer", WS_CHILD | WS_VISIBLE | ES_RIGHT | WS_BORDER | ES_READONLY,
			g_board_x, g_board_y - 30, 50, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hEdit_rest = CreateWindow(L"EDIT", L"step", WS_CHILD | WS_VISIBLE | ES_RIGHT | WS_BORDER | ES_READONLY,
			g_board_x + 60, g_board_y - 30, 50, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		InitGame(hWnd, g_level);
		break;
	case WM_COMMAND:
		wmId = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case ID_PRIMARY:
			if (g_level != PRIMARY)
			{
				OverGame(hWnd);
				g_level = PRIMARY;
				InitGame(hWnd, g_level);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case ID_MIDDLE:
			if (g_level != MIDDLE)
			{
				OverGame(hWnd);
				g_level = MIDDLE;
				InitGame(hWnd, g_level);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		case ID_ADVANCED:
			if (g_level != ADVANCED)
			{
				OverGame(hWnd);
				g_level = ADVANCED;
				InitGame(hWnd, g_level);
				InvalidateRect(hWnd, NULL, TRUE);
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
		break;
	case WM_LBUTTONDOWN:
		g_nCursor_x = (LOWORD(lParam) - g_board_x) / g_side_grid;
		g_nCursor_y = (HIWORD(lParam) - g_board_y) / g_side_grid;
		MouseLeft(hWnd);
		break;
	case WM_RBUTTONDOWN:
		g_nCursor_x = (LOWORD(lParam) - g_board_x) / g_side_grid;
		g_nCursor_y = (HIWORD(lParam) - g_board_y) / g_side_grid;
		MouseRight(hWnd);
		break;
	case WM_TIMER:
		wsprintf(g_cTimer, L"%ds", ++g_nTimer);
		SetWindowText(g_hEdit_timer, g_cTimer);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		memDc = CreateCompatibleDC(hdc);
		bmp = CreateCompatibleBitmap(hdc, g_nClient_x, g_nClient_y);
		SelectObject(memDc, bmp);
		DrawBK(memDc);
		DrawClient(memDc);
		BitBlt(hdc, 0, 0, g_nClient_x, g_nClient_y, memDc, 0, 0, SRCCOPY);
		DeleteObject(bmp);
		DeleteObject(memDc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
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

void AlignWindow(HWND hwnd, int x, int y)
{
	int screen_x, screen_y;
	int client_x, client_y;
	int proc_x, proc_y;
	int x1, y1;
	RECT rect;

	screen_x = GetSystemMetrics(SM_CXSCREEN);
	screen_y = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(hwnd, &rect);
	proc_x = rect.right - rect.left;
	proc_y = rect.bottom - rect.top;
	GetClientRect(hwnd, &rect);
	client_x = rect.right - rect.left;
	client_y = rect.bottom - rect.top;

	g_nClient_x = g_board_x * 2 + g_side_grid * x;
	g_nClient_y = g_board_y + g_board_x + g_side_grid * y;
	proc_x = proc_x - client_x + g_nClient_x;
	proc_y = proc_y - client_y + g_nClient_y;
	x1 = (screen_x - proc_x) / 2;
	y1 = (screen_y - proc_y) / 2;

	MoveWindow(hwnd, x1, y1, proc_x, proc_y, TRUE);
}

void DrawBK(HDC hdc)
{
	HPEN pen_background = CreatePen(BS_SOLID, 2, RGB(128, 128, 128));
	HPEN pen_grid = CreatePen(BS_SOLID, 1, RGB(0, 0, 255));
	int i;

	SelectObject(hdc, pen_background);
	Rectangle(hdc, g_board_x, g_board_y, g_board_x + g_side_grid*g_game->Get_side_x(), 
		g_board_y + g_side_grid*g_game->Get_side_y());
	DeleteObject(pen_background);

	SelectObject(hdc, pen_grid);
	for (i = 1; i < g_game->Get_side_x(); i++)
	{
		MoveToEx(hdc, g_board_x + g_side_grid*i, g_board_y, NULL);
		LineTo(hdc, g_board_x + g_side_grid*i, g_board_y + g_side_grid*g_game->Get_side_y());
	}
	for (i = 1; i < g_game->Get_side_y(); i++)
	{
		MoveToEx(hdc, g_board_x, g_board_y + g_side_grid*i, NULL);
		LineTo(hdc, g_board_x + g_side_grid*g_game->Get_side_x(), g_board_y + g_side_grid*i);
	}
	DeleteObject(pen_grid);
}

void InitGame(HWND hwnd, LEVEL l)
{
	switch (l)
	{
	case PRIMARY:
		g_game = new Game; 
		break;
	case MIDDLE:
		g_game = new Game(MIDDLE, 40, 16, 16, 256);
		break;
	case ADVANCED:
		g_game = new Game(ADVANCED, 99, 16, 30, 480);
		break;
	default:
		break;
	}

	g_nTimer = 0;
	wsprintf(g_cTimer, L"%ds", g_nTimer);
	wsprintf(g_cRest, L"%d", g_game->Get_rest());
	SetWindowText(g_hEdit_timer, g_cTimer);
	SetWindowText(g_hEdit_rest, g_cRest);
	SetTimer(hwnd, TIMER, 1000, NULL);
	AlignWindow(hwnd, g_game->Get_side_x(), g_game->Get_side_y());
}

void OverGame(HWND hwnd)
{
	KillTimer(hwnd, TIMER);
	g_game->Show_all();
	InvalidateRect(hwnd, NULL, TRUE);
	SendMessage(hwnd, WM_PAINT, 1, NULL);
	Sleep(50);
	delete g_game;
}

void MouseLeft(HWND hwnd)
{
	if (false == g_game->Left_button(g_nCursor_x, g_nCursor_y))
	{
		OverGame(hwnd);
		if (IDYES == MessageBox(hwnd, L"Ooh, you lost...Try again?", L"Result", MB_YESNO))
		{
			InitGame(hwnd, g_level);
			InvalidateRect(hwnd, NULL, TRUE);
		}
		else
			PostQuitMessage(0);
	}
	else
	{
		wsprintf(g_cRest, L"%d", g_game->Get_rest());
		SetWindowText(g_hEdit_rest, g_cRest);
		InvalidateRect(hwnd, NULL, TRUE);
		if (g_game->Get_rest() == 10)
		{
			OverGame(hwnd);
			if (IDYES == MessageBox(hwnd, L"Congratulations! You win!!! Play again?", L"Result", MB_YESNO))
			{
				InitGame(hwnd, g_level);
				InvalidateRect(hwnd, NULL, TRUE);
			}
			else
				PostQuitMessage(0);
		}
	}
}

void MouseRight(HWND hwnd)
{
	g_game->Right_button(g_nCursor_x, g_nCursor_y);
	InvalidateRect(hwnd, NULL, TRUE);
}

void DrawNum(HDC hdc, const int x, const int y)
{
	RECT rect;
	rect.left = g_board_x + g_side_grid*x;
	rect.right = g_board_x + g_side_grid*(x + 1);
	rect.top = g_board_y + g_side_grid*y;
	rect.bottom = g_board_y + g_side_grid*(y + 1);

	int **game_num = g_game->Get_client_num();
	wstring **game_char = g_game->Get_client_char();
	std::wstringstream ss;
	ss << game_num[y][x];
	game_char[y][x] = ss.str();
	SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, game_char[y][x].c_str(), 1, &rect, DT_CENTER | DT_VCENTER);
	game_num = nullptr;
	game_char = nullptr;
}

void DrawMine(HDC hdc, const int x, const int y)
{
	HDC tmpdc = CreateCompatibleDC(hdc);
	BITMAP bm;
	HBITMAP bmp = (HBITMAP)LoadImage(NULL, L"mine.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(bmp, sizeof(BITMAP), &bm);
	int x1 = g_board_x + g_side_grid*x;
	int y1 = g_board_y + g_side_grid*y;
	SelectObject(tmpdc, bmp);
	BitBlt(hdc, x1, y1, g_side_grid, g_side_grid, tmpdc, 0, 0, SRCCOPY);
	DeleteObject(tmpdc);
	DeleteObject(bmp);
}

void DrawFlag(HDC hdc, const int x, const int y)
{
	HDC tmpdc = CreateCompatibleDC(hdc);
	BITMAP bm;
	HBITMAP bmp = (HBITMAP)LoadImage(NULL, L"flag.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	GetObject(bmp, sizeof(BITMAP), &bm);
	int x1 = g_board_x + g_side_grid*x;
	int y1 = g_board_y + g_side_grid*y;
	SelectObject(tmpdc, bmp);
	BitBlt(hdc, x1, y1, g_side_grid, g_side_grid, tmpdc, 0, 0, SRCCOPY);
	DeleteObject(tmpdc);
	DeleteObject(bmp);
}

void DrawClient(HDC hdc)
{
	int i, j;
	int **game_flag = g_game->Get_client_flag();

	for (i = 0; i < g_game->Get_side_y(); i++)
	{
		for (j = 0; j < g_game->Get_side_x(); j++)
		{
			if (game_flag[i][j] == 0)
			{
				continue;
			}
			else if (game_flag[i][j] == 1)
			{
				DrawNum(hdc, j, i);
			}
			else if (game_flag[i][j] == 2)
			{
				DrawFlag(hdc, j, i);
			}
			else if (game_flag[i][j] == 3)
			{
				DrawMine(hdc, j, i);
			}
		}
	}
	game_flag = nullptr;
}