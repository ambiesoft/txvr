// txvr.cpp : Defines the entry point for the application.
//

#include "framework.h"
#include "../../lsMisc/CHandle.h"
#include "txvr.h"
#pragma comment(lib, "Shlwapi.lib")

using namespace std;
using namespace Ambiesoft;
//using namespace Ambiesoft::stdosd;

// Global Variables:
HINSTANCE ghInst;                                // current instance
CFileHandle ghFile;
HWND ghEdit;
HWND ghEditWR;
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")