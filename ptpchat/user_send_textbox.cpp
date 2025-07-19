#include "user_send_textbox.h"


WNDPROC UserSendTextbox::defaultProc = nullptr;

UserSendTextbox::UserSendTextbox(HWND parentWindow, HINSTANCE parentInstance, HMENU controlId)  {

    this->handle = CreateWindowEx(
        WS_EX_CLIENTEDGE, "EDIT", "",
        WS_CHILD | WS_VISIBLE | ES_LEFT | WS_VSCROLL | ES_MULTILINE,
        40, 475, 600, 50,
        parentWindow, controlId, parentInstance, NULL
    );

    defaultProc = (WNDPROC)SetWindowLongPtr(this->handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(UserSendTextbox::Proc));
}

LRESULT CALLBACK UserSendTextbox::Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (message == WM_KEYDOWN) {

        switch (wParam) {
            case VK_RETURN:
            {
                bool shiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;

                if (!shiftHeld) {
                    TCHAR buffer[BUFFER_SIZE];
                    GetWindowText(hWnd, buffer, BUFFER_SIZE);

                    std::string* message = new std::string(buffer);
                    if (!Network::BroadcastMessage(message)) {
                        SetWindowTextW(hWnd, L"");
                    }

                    return 0;
                }
                break;
            }
        }
    }

    if (message == WM_CHAR) {
        switch (wParam) {
            case VK_RETURN:
            {
                bool shiftHeld = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                if (shiftHeld) {
                    int len = GetWindowTextLengthW(hWnd);
                    SendMessageW(hWnd, EM_SETSEL, len, len);
                    std::wstring newline = L"\r\n";
                    SendMessageW(hWnd, EM_REPLACESEL, FALSE, reinterpret_cast<LPARAM>(newline.c_str()));
                }
                return 0;
            }
        }
    }

    // Pass everything else back to the original edit control logic
    return CallWindowProc(defaultProc, hWnd, message, wParam, lParam);
}