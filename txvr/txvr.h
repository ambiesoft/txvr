#pragma once

#include "resource.h"
#include "../../lsMisc/CHandle.h"

#define APPNAME L"txvr"
#define I18N(s) s

#define MAX_LOADSTRING 100

// 10mb
#define MAX_FILE_SIZE  (10 * 1024 * 1024)
#define SECTION_LOCATION "Location"
#define KEY_PLACEMENT "Placement"


// Global Variables:
extern HINSTANCE ghInst;                                // current instance
extern Ambiesoft::CFileHandle ghFile;
extern HWND ghEdit;
extern WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
extern WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void SaveSettings(HWND hWnd);