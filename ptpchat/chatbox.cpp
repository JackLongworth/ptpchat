#include "chatbox.h"

std::string Chatbox::gText;
HWND Chatbox::handle = nullptr;
WNDPROC Chatbox::defaultProc = nullptr;

typedef struct chat_message {
    char buffer[4096];
    char author[24];
} ChatMessage;

std::vector<ChatMessage> messageQueue;

Chatbox::Chatbox(HWND parentWindow, HINSTANCE parentInstance) {

	Chatbox::handle = CreateWindowExW(
        0,
        MSFTEDIT_CLASS,                    // L"RICHEDIT50W"
        L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | WS_BORDER,
        40, 40, 600, 400,                  // Position and size
        parentWindow,                        // Parent window
        nullptr,                           // No menu
        parentInstance,                         // Application instance
        nullptr
    );

    defaultProc = (WNDPROC)SetWindowLongPtr(this->handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(Chatbox::Proc));
}

LRESULT CALLBACK Chatbox::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

    switch (message) {
        case WM_SETCURSOR:
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
            return TRUE;
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_MOUSEACTIVATE:
            return MA_NOACTIVATE; // Prevent focus and selection
        case WM_APPEND_TEXT:
        {
            std::string* pText = reinterpret_cast<std::string*>(lParam);
            Chatbox::AppendText(*pText);
            delete pText;
            break;
        }

    }

    return CallWindowProc(defaultProc, hWnd, message, wParam, lParam);
}

void Chatbox::AppendText(const std::string& text) {
    if (!IsWindow(Chatbox::handle)) return;

    std::wstring wText = Chatbox::ToWString(text);

    // Move caret to end
    const int len = GetWindowTextLengthW(Chatbox::handle);
    SendMessageW(Chatbox::handle, EM_SETSEL, len, len);
    SendMessageW(Chatbox::handle, EM_HIDESELECTION, FALSE, 0);

    // Append text + newline
    std::wstring textWithNewline = wText + L"\r\n";

    SendMessageW(Chatbox::handle, EM_REPLACESEL, FALSE, (LPARAM)textWithNewline.c_str());

    SendMessageW(Chatbox::handle, EM_SCROLLCARET, 0, 0);
}

std::wstring Chatbox::ToWString(const std::string& str) {
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    std::wstring wstr(size - 1, 0); // exclude null terminator
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}
