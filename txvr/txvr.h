#pragma once

#include "resource.h"
#include "../../lsMisc/CHandle.h"

#define APPNAME L"txvr"
#define I18N(s) s

#define MAX_LOADSTRING 100

// 10mb
#define MAX_FILE_SIZE  (10 * 1024 * 1024)
#define SECTION_LOCATION "Location"
#define SECTION_OPTION "Option"
#define KEY_PLACEMENT "Placement"
#define KEY_WORDWRAP "Wordwrap"
#define KEY_LOGFONT "LogFont"


// Global Variables:
extern HINSTANCE ghInst;                                // current instance
extern Ambiesoft::CFileHandle ghFile;
extern HWND ghEdit;
extern HWND ghEditWR;
extern WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
extern WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void SaveSettings(HWND hWnd, HWND hEdit);
bool getLogFontFromWindow(HWND hWnd, LOGFONT* pLogFont);

struct CreateWinStruct
{
	bool bWordWrap_;
	LOGFONT* pLogFont_;

	CreateWinStruct() {
		ZeroMemory(this, sizeof(*this));
	}
	~CreateWinStruct() {
		delete pLogFont_;
		pLogFont_ = nullptr;
	}
};