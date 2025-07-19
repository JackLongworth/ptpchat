#pragma once
#include "framework.h"
#include "Resource.h"
#include <string>

class JoinDialog {

public:
	JoinDialog(HINSTANCE hInstance, HWND hWndParent);

private: 
	static INT_PTR CALLBACK InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
};