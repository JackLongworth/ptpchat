#pragma once

#include "framework.h"
#include <vector>


#define BUFFER_SIZE 1024

class Chatbox {
	
public:
	Chatbox(HWND parentWindow, HINSTANCE parentInstance);
private:
	static LRESULT CALLBACK    Proc(HWND, UINT, WPARAM, LPARAM);
	static void AppendText(const std::string& text);
	static std::wstring ToWString(const std::string& std);

public:
	static HWND handle;

private:
	static WNDPROC defaultProc;
	static std::string gText;
};