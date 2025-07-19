// header.h : include file for standard system include files,
// or project specific include files
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <ws2tcpip.h>
#include <WinSock2.h>
#include <windows.h>
#include <commctrl.h>
#include <Richedit.h>
#include <Richole.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>


// definitions
#define WM_SOCKET (WM_USER + 1)
#define WM_APPEND_TEXT (WM_USER + 2)
#define WM_USER_HOST_IP_INPUT (WM_USER + 3)