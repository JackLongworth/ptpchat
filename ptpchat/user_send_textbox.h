#pragma once

#include "framework.h"
#include <functional>
#include "network.h"

#define BUFFER_SIZE 1024

class UserSendTextbox
{
public:
	UserSendTextbox(HWND parentWindow, HINSTANCE parentInstance, HMENU controlId);
	// , std::function<void(char*)> onEnter
	
private:
	static LRESULT CALLBACK    Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	HMENU controlId;

private:
	static WNDPROC defaultProc;

	HWND handle;
	std::function<void(std::string)> onEnter;
};

