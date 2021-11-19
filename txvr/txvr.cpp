// txvr.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include <string>
#include "../../lsMisc/CommandLineParser.h"
#include "../../lsMisc/stdosd/stdosd.h"
#include "../../lsMisc/HighDPI.h"
#include "../../lsMisc/CHandle.h"
#include "../../lsMisc/GetLastErrorString.h"
#include "../../lsMisc/DebugMacro.h"
#include "../../lsMisc/GetVersionString.h"
#include "../../profile/cpp/Profile/include/ambiesoft.profile.h"
#include "txvr.h"

#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;

#define MAX_LOADSTRING 100

// 10mb
#define MAX_FILE_SIZE  (10 * 1024 * 1024)
#define SECTION_LOCATION "Location"
#define KEY_PLACEMENT "Placement"

// Global Variables:
HINSTANCE ghInst;                                // current instance
CFileHandle ghFile;
HWND ghEdit;

WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void ErrorExit(LPCTSTR pMessage)
{
	MessageBox(NULL, pMessage, APPNAME, MB_ICONERROR);
	ExitProcess(1);
}
void ErrorExit(const wstring& message)
{
	ErrorExit(message.c_str());
}
void ErrorExit(DWORD le)
{
	ErrorExit(GetLastErrorString(le).c_str());
}
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	InitHighDPISupport();

	CCommandLineParser parser;
	wstring file;
	parser.AddOption(L"", ArgCount::ArgCount_OneToInfinite, &file, ArgEncodingFlags::ArgEncodingFlags_Default,
		I18N(L"File to open"));

	parser.Parse();

	if (file.empty()) {
		ErrorExit(I18N(L"No file specified."));
	}
	if (!PathFileExists(file.c_str())) {
		ErrorExit(stdFormat(I18N(L"'%s' does not exists."), file.c_str()));
	}

	ghFile = CreateFile(
		file.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (!ghFile)
	{
		ErrorExit(GetLastErrorString(GetLastError()).c_str());
	}

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_TXVR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TXVR));

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

	return (int)msg.wParam;
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

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TXVR));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TXVR);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

wstring GetIniFilePath()
{
	return stdCombinePath(
		stdGetParentDirectory(stdGetModuleFileName()),
		stdGetFileNameWitoutExtension(stdGetModuleFileName()) + L".ini");
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

	ghInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	Profile::CHashIni ini(Profile::ReadAll(GetIniFilePath()));
	vector<BYTE> vwpm;
	if (Profile::GetBinary(SECTION_LOCATION, KEY_PLACEMENT, vwpm, ini))
	{
		if (vwpm.size() == sizeof(WINDOWPLACEMENT))
		{
			DVERIFY(SetWindowPlacement(hWnd, (const WINDOWPLACEMENT*)&vwpm[0]));
		}
	}

	ShowWindow(hWnd, nCmdShow);
	DVERIFY(UpdateWindow(hWnd));

	return TRUE;
}
void SaveSettings(HWND hWnd)
{
	Profile::CHashIni ini(Profile::ReadAll(GetIniFilePath()));

	WINDOWPLACEMENT wpm = { 0 };
	wpm.length = sizeof(wpm);
	if (GetWindowPlacement(hWnd, &wpm))
	{
		Profile::WriteBinary(SECTION_LOCATION, KEY_PLACEMENT,
			(const LPBYTE)&wpm, sizeof(wpm), ini);
	}
	if (!Profile::WriteAll(ini, GetIniFilePath()))
	{
		MessageBox(hWnd,
			I18N(L"Failed to save ini"),
			APPNAME,
			MB_ICONWARNING);
	}
}
wstring GetAllText(HANDLE hFile)
{
	LARGE_INTEGER fileSize;
	fileSize.QuadPart = 0;
	if (!GetFileSizeEx(hFile, &fileSize))
	{
		ErrorExit(GetLastError());
	}
	if (fileSize.QuadPart > MAX_FILE_SIZE)
	{
		ErrorExit(I18N(L"File size is too big."));
	}
	vector<BYTE> v;
	v.resize(fileSize.LowPart + 4);
	DWORD dwRead = 0;
	if (!ReadFile(hFile,
		&v[0],
		fileSize.LowPart,
		&dwRead,
		NULL) || dwRead != fileSize.LowPart)
	{
		ErrorExit(GetLastError());
	}
	v[fileSize.LowPart] = 0;
	v[fileSize.LowPart + 1] = 0;
	v[fileSize.LowPart + 2] = 0;
	v[fileSize.LowPart + 3] = 0;
	return toStdWstringFromUtf8((const char*)&v[0]);
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
	case WM_CREATE:
	{
		RECT r;
		GetClientRect(hWnd, &r);
		ghEdit = CreateWindow(
			TEXT("EDIT"), TEXT(""),
			WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_READONLY | ES_WANTRETURN,
			r.left, r.top, r.right - r.left, r.bottom - r.top,
			hWnd, (HMENU)1,
			ghInst, NULL);

		LOGFONT lfText = {};
		SystemParametersInfo(SPI_GETICONTITLELOGFONT,
			sizeof(lfText),
			&lfText,
			FALSE);
		HFONT hFontNew = CreateFontIndirect(&lfText);
		SendMessage(ghEdit, WM_SETFONT, (WPARAM)hFontNew, FALSE);
		wstring all = GetAllText(ghFile);
		SetWindowText(ghEdit, all.c_str());
	}
	break;

	case WM_SIZE:
	{
		RECT r;
		GetClientRect(hWnd, &r);
		MoveWindow(ghEdit, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
	}
	break;

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
		SaveSettings(hWnd);
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
		{
			wstring appversion = stdFormat(L"%s v%s",
				APPNAME,
				GetVersionString(NULL, 3).c_str());
			DVERIFY(SetDlgItemText(hDlg, IDC_STATIC_APPVERSION, appversion.c_str()));
		}
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
