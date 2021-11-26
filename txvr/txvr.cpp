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
#include "../../lsMisc/ErrorExit.h"
#include "../../profile/cpp/Profile/include/ambiesoft.profile.h"
#include "txvr.h"

#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);



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

	Profile::CHashIni ini(Profile::ReadAll(GetIniFilePath()));

	CreateWinStruct cws = { 0 };
	bool vbal;
	Profile::GetBool(SECTION_OPTION, KEY_WORDWRAP, false, vbal, ini);
	cws.bWordWrap_ = vbal;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, &cws);

	if (!hWnd)
	{
		return FALSE;
	}

	
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

	MENUITEMINFO mii;
	ZeroMemory(&mii, sizeof(mii));
	mii.cbSize = sizeof(mii);
	mii.fMask = MIIM_STATE;
	DVERIFY(GetMenuItemInfo(GetMenu(hWnd), ID_TOOLS_WORDRAP, FALSE, &mii));
	Profile::WriteBool(SECTION_OPTION, KEY_WORDWRAP,
		(mii.fState & MFS_CHECKED) ? true : false,
		ini);

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
