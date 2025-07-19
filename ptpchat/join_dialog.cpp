#include "join_dialog.h"

JoinDialog::JoinDialog(HINSTANCE hInstance, HWND hWndParent) {
    DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_JOIN_DIALOG), hWndParent, JoinDialog::InputDlgProc, reinterpret_cast<LPARAM>(hWndParent));
}

INT_PTR CALLBACK JoinDialog::InputDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
    static HWND hWndParent = nullptr;

    switch (message) {
    case WM_INITDIALOG:
        hWndParent = reinterpret_cast<HWND>(lParam);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case IDOK:
        {

            char inputIP[INET_ADDRSTRLEN];
            GetDlgItemText(hDlg, IDC_EDIT, inputIP, INET_ADDRSTRLEN);

            std::string* joinIP = new std::string(inputIP);

            SendMessage(hWndParent, WM_USER_HOST_IP_INPUT, 0, reinterpret_cast<LPARAM>(joinIP));

            EndDialog(hDlg, IDOK);
            return (INT_PTR)TRUE;
        }
        case IDCANCEL:
            EndDialog(hDlg, IDCANCEL);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
