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
#include "../../lsMisc/GetAllTexts.h"
#include "../../lsMisc/ErrorExit.h"
#include "../../lsMisc/menu.h"
#include "../../profile/cpp/Profile/include/ambiesoft.profile.h"

#include "helper.h"
#include "txvr.h"

#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using namespace Ambiesoft;
using namespace Ambiesoft::stdosd;


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
		// check menus
		CreateWinStruct* cws = (CreateWinStruct*)((CREATESTRUCT*)lParam)->lpCreateParams;


		RECT r;
		GetClientRect(hWnd, &r);
		LONG style = WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL |
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_NOHIDESEL | ES_READONLY | ES_WANTRETURN;

		ghEdit = CreateWindow(
			TEXT("EDIT"), TEXT(""),
			style,
			r.left, r.top, r.right - r.left, r.bottom - r.top,
			hWnd, (HMENU)1,
			ghInst, NULL);
		
		style ^= ES_AUTOHSCROLL | WS_HSCROLL;
		ghEditWR = CreateWindow(
			TEXT("EDIT"), TEXT(""),
			style,
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
		SendMessage(ghEditWR, WM_SETFONT, (WPARAM)hFontNew, FALSE);

		LARGE_INTEGER fileSize;
		fileSize.QuadPart = 0;
		if (!GetFileSizeEx(ghFile, &fileSize))
		{
			ErrorExit(GetLastError());
		}
		if (fileSize.QuadPart > MAX_FILE_SIZE)
		{
			ErrorExit(I18N(L"File size is too large"));
		}
		vector<BYTE> all = GetAllTexts(ghFile, fileSize.LowPart);
		//int cp = GetDetectedCodecGoogle(&all[0], all.size());
		wstring allText = GetDetectedCodecICU(&all[0], (int)all.size());
		// wstring allText = toStdWstring(cp, (const char*)&all[0], all.size());
		SetWindowText(ghEdit, allText.c_str());
		SetWindowText(ghEditWR, allText.c_str());

		DVERIFY(CheckMenu(GetMenu(hWnd), ID_TOOLS_WORDRAP, cws->bWordWrap_));
		if (cws->bWordWrap_)
			ShowWindow(ghEdit, SW_HIDE);
		else
			ShowWindow(ghEditWR, SW_HIDE);
	}
	break;

	case WM_SIZE:
	{
		RECT r;
		GetClientRect(hWnd, &r);
		MoveWindow(ghEdit, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
		MoveWindow(ghEditWR, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
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
		case ID_TOOLS_WORDRAP:
		{
			bool bOld = IsMenuChecked(GetMenu(hWnd), ID_TOOLS_WORDRAP);
			if(bOld)
			{
				ShowWindow(ghEdit, SW_SHOW);
				ShowWindow(ghEditWR, SW_HIDE);
			}
			else
			{
				ShowWindow(ghEditWR, SW_SHOW);
				ShowWindow(ghEdit, SW_HIDE);
			}
			DVERIFY(CheckMenu(GetMenu(hWnd), ID_TOOLS_WORDRAP, !bOld));
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
		SaveSettings(hWnd);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

