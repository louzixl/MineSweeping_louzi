// MineSweeping.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MineSweeping.h"
#include <cstdio>
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>

using std::vector;

#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

//自定义结构体
enum LEVEL{JUNIOR, MIDDLE, ADVANCED};

struct GameStruct{
	int side_x;
	int side_y;
	int num_of_mine;
	int **game_area;
};

//自定义全局变量
GameStruct g_setting;
const int g_board_x = 10;
const int g_board_y = 60;
const int g_side_grid = 20;
HWND g_hStatic_num_flag, g_hStatic_timer, g_hStatic_step;
HWND g_hEdit_num_flag, g_hEdit_timer, g_hEdit_step;
int g_nNum_flag;
int g_nTimer;
int g_nStep;
wchar_t g_cNum_flag[10];
wchar_t g_cTimer[10];
wchar_t g_cStep[10];
const int TIMER = 1;
vector<int> g_vMine;
POINT g_mouse_pos;
wchar_t **g_cGame_area;
int g_nRest;

//函数声明
void AlignWindow(HWND hwnd);
void DrawBK(HDC hdc);
void InitGame(HWND hwnd, LEVEL l);
void AllocMem();
void InitMinePos();
int Random(int max);
void InitMap();
void AddNum(int i, int j);
void OverGame(HWND hwnd);
void MouseLeft(HWND hwnd, HDC hdc, const POINT &p);
void DrawNum(HDC hdc, int x, int y);
void DrawMine(HDC hdc, int x, int y);
void DrawFlag(HDC hdc, int x, int y);

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
	if (!InitInstance (hInstance, nCmdShow))
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

	return (int) msg.wParam;
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

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEEPING));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MINESWEEPING);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		g_hStatic_num_flag = CreateWindow(L"STATIC", L"Flag", WS_CHILD | WS_VISIBLE | SS_CENTER,
			g_board_x, g_board_y - 50, 50, 15, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hStatic_timer = CreateWindow(L"STATIC", L"Timer", WS_CHILD | WS_VISIBLE | SS_CENTER,
			g_board_x + 60, g_board_y - 50, 50, 15, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hStatic_step = CreateWindow(L"STATIC", L"Step", WS_CHILD | WS_VISIBLE | SS_CENTER,
			g_board_x + 120, g_board_y - 50, 50, 15, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hEdit_num_flag = CreateWindow(L"EDIT", L"mine", WS_CHILD | WS_VISIBLE | ES_RIGHT | WS_BORDER | ES_READONLY,
			g_board_x, g_board_y - 30, 50, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hEdit_timer = CreateWindow(L"EDIT", L"timer", WS_CHILD | WS_VISIBLE | ES_RIGHT | WS_BORDER | ES_READONLY,
			g_board_x + 60, g_board_y - 30, 50, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		g_hEdit_step = CreateWindow(L"EDIT", L"step", WS_CHILD | WS_VISIBLE | ES_RIGHT | WS_BORDER | ES_READONLY,
			g_board_x + 120, g_board_y - 30, 50, 20, hWnd, NULL, GetModuleHandle(NULL), NULL);
		InitGame(hWnd, MIDDLE);
		AlignWindow(hWnd);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
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
		GetCursorPos(&g_mouse_pos);

		break;
	case WM_TIMER:
		wsprintf(g_cTimer, L"%ds", ++g_nTimer);
		SetWindowText(g_hEdit_timer, g_cTimer);
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO:  在此添加任意绘图代码...
		DrawBK(hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		OverGame(hWnd);
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

void AlignWindow(HWND hwnd)
{
	int screen_x, screen_y;
	int client_x, client_y;
	int proc_x, proc_y;
	int x, y;
	RECT rect;

	screen_x = GetSystemMetrics(SM_CXSCREEN);
	screen_y = GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(hwnd, &rect);
	proc_x = rect.right - rect.left;
	proc_y = rect.bottom - rect.top;
	GetClientRect(hwnd, &rect);
	client_x = rect.right - rect.left;
	client_y = rect.bottom - rect.top;

	proc_x = proc_x - client_x + g_board_x * 2 + g_side_grid*g_setting.side_x;
	proc_y = proc_y - client_y + g_board_y + g_board_x + g_side_grid*g_setting.side_y;
	x = (screen_x - proc_x) / 2;
	y = (screen_y - proc_y) / 2;

	MoveWindow(hwnd, x, y, proc_x, proc_y, TRUE);
}

void DrawBK(HDC hdc)
{
	HPEN pen_background = CreatePen(BS_SOLID, 2, RGB(128, 128, 128));
	HPEN pen_grid = CreatePen(BS_SOLID, 1, RGB(0, 0, 255));
	int i, j;

	SelectObject(hdc, pen_background);
	Rectangle(hdc, g_board_x, g_board_y, g_board_x + g_side_grid*g_setting.side_x, g_board_y + g_side_grid*g_setting.side_y);
	DeleteObject(pen_background);

	SelectObject(hdc, pen_grid);
	for (i = 1; i < g_setting.side_x; i++)
	{
		MoveToEx(hdc, g_board_x + g_side_grid*i, g_board_y, NULL);
		LineTo(hdc, g_board_x + g_side_grid*i, g_board_y + g_side_grid*g_setting.side_y);
	}
	for (i = 1; i < g_setting.side_y; i++)
	{
		MoveToEx(hdc, g_board_x, g_board_y + g_side_grid*i, NULL);
		LineTo(hdc, g_board_x + g_side_grid*g_setting.side_x, g_board_y + g_side_grid*i);
	}
	/*for (i = 0; i < g_setting.side_y; i++)
	{
		for (j = 0; j < g_setting.side_x; j++)
		{
			Rectangle(hdc, g_board_x + g_side_grid*j, g_board_y + g_side_grid*i,
				g_board_x + g_side_grid*(j + 1), g_board_y + g_side_grid*(i + 1));
		}
	}*/
	DeleteObject(pen_grid);
}

void InitGame(HWND hwnd, LEVEL l)
{
	int i, j;
	switch (l)
	{
	case JUNIOR:
		g_setting.num_of_mine = 10;
		g_setting.side_x = g_setting.side_y = 9;
		AllocMem();
		break;
	case MIDDLE:
		g_setting.num_of_mine = 40;
		g_setting.side_x = g_setting.side_y = 16;
		AllocMem();
		break;
	case ADVANCED:
		g_setting.num_of_mine = 99;
		g_setting.side_x = 16;
		g_setting.side_y = 30;
		AllocMem();
		break;
	default: 
		break;
	}
	g_nRest = g_setting.side_x*g_setting.side_y;
	g_nNum_flag = 0;
	g_nTimer = 0;
	g_nStep = 0;
	wsprintf(g_cNum_flag, L"%d", g_nNum_flag);
	wsprintf(g_cTimer, L"%ds", g_nTimer);
	wsprintf(g_cStep, L"%d", g_nStep);
	SetWindowText(g_hEdit_num_flag, g_cNum_flag);
	SetWindowText(g_hEdit_timer, g_cTimer);
	SetWindowText(g_hEdit_step, g_cStep);

	InitMinePos();
	InitMap();
	SetTimer(hwnd, TIMER, 1000, NULL);
}

void AllocMem()
{
	int i;

	g_setting.game_area = new int*[g_setting.side_y];
	g_cGame_area = new wchar_t*[g_setting.side_y];
	for (i = 0; i < g_setting.side_y; i++)
	{
		g_setting.game_area[i] = new int[g_setting.side_x];
		g_cGame_area[i] = new wchar_t[g_setting.side_x];
		memset(g_setting.game_area[i], 0, sizeof(g_setting.game_area[i]));
		memset(g_cGame_area[i], 0, sizeof(g_cGame_area[i]));
	}
}

void InitMinePos()
{
	int i;
	int tmp_pos, col, row;
	bool bFlag = false;
	int total_grid = g_setting.side_x*g_setting.side_y;

	for (i = 0; i < g_setting.num_of_mine; i++)
	{
		g_vMine.push_back(Random(total_grid - 1));
	}
	while (1)
	{
		bFlag = false;
		sort(g_vMine.begin(), g_vMine.end());
		vector<int>::iterator pos = g_vMine.begin();
		vector<int>::const_iterator end = g_vMine.cend();
		for (pos; pos != end - 1; ++pos)
		{
			if (*pos == *(pos + 1))
			{
				*pos = Random(total_grid - 1);
				bFlag = true;
				break;
			}
		}
		if (pos == end - 1 && !bFlag)
			break;
	}
	for (i = 0; i < g_setting.num_of_mine; i++)
	{
		tmp_pos = g_vMine[i];
		row = tmp_pos / g_setting.side_y;
		col = tmp_pos % g_setting.side_y;
		g_setting.game_area[row][col] = -1;
	}
}

int Random(int max)
{
	std::default_random_engine engine(time(0));
	return std::uniform_int_distribution<int>(0, max)(engine);
}

void InitMap()
{
	int i, j;
	int row, col;
	vector<int>::const_iterator pos = g_vMine.cbegin();
	vector<int>::const_iterator end = g_vMine.cend();

	for (pos; pos != end; ++pos)
	{
		row = (*pos) / g_setting.side_y;
		col = (*pos) % g_setting.side_y;
		AddNum(row, col);
	}
}

void AddNum(int i, int j)
{
	if (i > 0 && g_setting.game_area[i - 1][j] != -1)
	{
		g_setting.game_area[i-1][j]++;
		if (j > 0 && g_setting.game_area[i - 1][j - 1] != -1)
			g_setting.game_area[i - 1][j - 1]++;
		if (j < g_setting.side_x - 1 && g_setting.game_area[i - 1][j + 1] != -1)
			g_setting.game_area[i - 1][j + 1]++;
	}
	if (i < g_setting.side_y - 1 && g_setting.game_area[i + 1][j] != -1)
	{
		g_setting.game_area[i + 1][j]++;
		if (j > 0 && g_setting.game_area[i + 1][j - 1] != -1)
			g_setting.game_area[i + 1][j - 1]++;
		if (j < g_setting.side_x - 1 && g_setting.game_area[i + 1][j + 1] != -1)
			g_setting.game_area[i + 1][j + 1]++;
	}
	if (j > 0 && g_setting.game_area[i][j - 1] != -1)
		g_setting.game_area[i][j - 1]++;
	if (j < g_setting.side_x - 1 && g_setting.game_area[i][j + 1] != -1)
		g_setting.game_area[i][j + 1]++;
}

void OverGame(HWND hwnd)
{
	KillTimer(hwnd, TIMER);
	int i, j;

	for (i = 0; i < g_setting.side_y; i++)
	{
		delete[]g_setting.game_area[i];
	}
	delete[] g_setting.game_area;
	for (i = 0; i < g_setting.side_y; i++)
	{
		delete[]g_cGame_area[i];
	}
	delete[] g_cGame_area;
}

void MouseLeft(HWND hwnd, HDC hdc, const POINT &p)
{
	if (p.x<g_board_x || p.x>=g_board_x + g_side_grid*g_setting.side_x ||
		p.y<g_board_y || p.y>=g_board_y + g_side_grid*g_setting.side_y)
		return;
	int x = p.x;
	int y = p.y;
	int col = (x - g_board_x) / g_side_grid;
	int row = (y - g_board_y) / g_side_grid;
	if (g_setting.game_area[row][col] >= 0)
	{
		DrawNum(hdc, col, row);
		g_nRest--;
		g_nStep++;
		wsprintf(g_cStep, L"%d", g_nStep);
		SetWindowText(g_hEdit_step, g_cStep);
		if (g_nRest <= 10)
		{
			if (IDYES == MessageBox(hwnd, L"Congratulations! You win!", L"Result", MB_YESNO))
			{
				InitGame(hwnd, JUNIOR);
				AlignWindow(hwnd);
			}
			else
				PostQuitMessage(0);
		}
	}
	else if (g_setting.game_area[row][col] == -1)
	{
		DrawMine(hdc, col, row);
		OverGame(hwnd);
		if (IDYES == MessageBox(hwnd, L"Ooh, you lost...", L"Result", MB_YESNO))
		{
			InitGame(hwnd, JUNIOR);
			AlignWindow(hwnd);
		}
		else
			PostQuitMessage(0);
	}
	else if (g_setting.game_area[row][col] == -2)
	{
		DrawFlag(hdc, col, row);
		g_nNum_flag++;
		wsprintf(g_cNum_flag, L"%d", g_nNum_flag);
		SetWindowText(g_hEdit_num_flag, g_cNum_flag);
	}
}

void DrawNum(HDC hdc, int x, int y)
{

}

void DrawMine(HDC hdc, int x, int y)
{

}

void DrawFlag(HDC hdc, int x, int y)
{

}