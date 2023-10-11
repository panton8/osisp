#include <windows.h>
#include <WinBase.h>
#include <psapi.h>
#include <vector>
#include <string>
#include <commctrl.h>
#include <shlwapi.h>
#include <algorithm>
#include "utility.h"

#define CLASS_NAME "MyWindowClass"
#define IDC_PROCESS_LISTVIEW 101
#define IDC_UPDATE_BUTTON 102
#define IDC_PAUSE_BUTTON 200
#define IDC_RESUME_BUTTON 201
#define IDC_TERMINATE_BUTTON 202
#define IDC_PROCESS_SEARCH_EDIT 300
#define IDC_PROCESS_SEARCH_BUTTON 301

void AddProcessesToListView(HWND listView, std::string searchQuery) {
    DWORD aProcesses[1024], cbNeeded, cProcesses;
    EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded);
    cProcesses = cbNeeded / sizeof(DWORD);

    LVITEM lvI;
    lvI.mask = LVIF_TEXT;
    lvI.state = 0;
    lvI.stateMask = 0;

    char buffer[1024];
    int itemCount = 0;

    std::string searchQueryLower = toLowerCase(searchQuery);

    for (DWORD i = 0; i < cProcesses; i++) {
        SIZE_T workingSetSize;
        SIZE_T privateUsage;

        GetProcessMemoryUsage(aProcesses[i], workingSetSize, privateUsage);

        std::string processName = GetProcessName(aProcesses[i]);

         // Filter by searchQuery
        if(processName.empty() || (searchQueryLower[0] != '\0' && toLowerCase(processName).find(searchQueryLower) == std::string::npos ))
            continue;

        lvI.iItem = itemCount;

        lvI.iSubItem = 0;
        wsprintf(buffer, "%d", aProcesses[i]);
        lvI.pszText = buffer;
        ListView_InsertItem(listView, &lvI);

        lvI.iSubItem = 1;
        strncpy(buffer, processName.c_str(), sizeof(buffer) / sizeof(buffer[0]));
        ListView_SetItemText(listView, itemCount, 1, buffer);

        lvI.iSubItem = 2;
        sprintf_s(buffer, sizeof(buffer), "%.2lf", (double)workingSetSize / (1024 * 1024));
        ListView_SetItemText(listView, itemCount, 2, buffer);

        ++itemCount;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND listView = NULL;
    static HWND button = NULL;

    switch (uMsg) {
    case WM_CREATE:
        {
            INITCOMMONCONTROLSEX icex;
            icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_STANDARD_CLASSES;
            InitCommonControlsEx(&icex);

            DWORD listViewStyles = WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS;

            listView = CreateWindowEx(0, WC_LISTVIEW, "", listViewStyles,
                0, 0, 500, 500, hwnd, (HMENU)IDC_PROCESS_LISTVIEW,
                GetModuleHandle(NULL), NULL);

            // Устанавливаем стиль "выделение полной строки"
            ListView_SetExtendedListViewStyle(listView, LVS_EX_FULLROWSELECT);

            HFONT hFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
                            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                            DEFAULT_PITCH | FF_ROMAN, "Arial");

            SendMessage(listView, WM_SETFONT, reinterpret_cast<WPARAM>(hFont), TRUE);

            LVCOLUMN lvc;
            lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
            lvc.fmt = LVCFMT_CENTER;

            lvc.iSubItem = 0;
            lvc.pszText = (LPCSTR)"ID"; // type cast to LPCSTR
            lvc.cx = 100;
            ListView_InsertColumn(listView, 0, &lvc);

            lvc.iSubItem = 1;
            lvc.pszText = (LPCSTR)"Name"; // type cast to LPCSTR
            lvc.cx = 600;
            ListView_InsertColumn(listView, 1, &lvc);

            lvc.iSubItem = 2;
            lvc.pszText = (LPCSTR)"Space used in memory (MB)"; // type cast to LPCSTR
            lvc.cx = 345;
            ListView_InsertColumn(listView, 2, &lvc);

            button = CreateWindow("BUTTON", "Update", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                  1055, 0, 70, 30, hwnd, (HMENU)IDC_UPDATE_BUTTON, GetModuleHandle(NULL), NULL);

            // Create Pause button
            CreateWindow("BUTTON", "Pause", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                 1055, 60, 70, 30, hwnd, (HMENU)IDC_PAUSE_BUTTON, GetModuleHandle(NULL), NULL);

            // Create Resume button
            CreateWindow("BUTTON", "Resume", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                 1055, 100, 70, 30, hwnd, (HMENU)IDC_RESUME_BUTTON, GetModuleHandle(NULL), NULL);

            // Create Terminate button
            CreateWindow("BUTTON", "Terminate", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                 1055, 140, 70, 30, hwnd, (HMENU)IDC_TERMINATE_BUTTON, GetModuleHandle(NULL), NULL);

            hwndEdit = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                        1055, 100, 500, 70, hwnd, (HMENU)IDC_PROCESS_SEARCH_EDIT,
                        GetModuleHandle(NULL), NULL);

            CreateWindow("BUTTON", "Find", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
             1155, 275, 60, 30, hwnd, (HMENU)IDC_PROCESS_SEARCH_BUTTON,
             GetModuleHandle(NULL), NULL);
        }
        break;

case WM_SIZE:
{
    int buttonXPos;
    int buttonYPos = 50;

    buttonXPos = LOWORD(lParam)-80;
    SetWindowPos(button, NULL, buttonXPos, buttonYPos, 70, 30, SWP_NOZORDER);

    buttonYPos += 40;
    SetWindowPos(GetDlgItem(hwnd, IDC_PAUSE_BUTTON), NULL, buttonXPos, buttonYPos, 70, 30, SWP_NOZORDER);

    buttonYPos += 40;
    SetWindowPos(GetDlgItem(hwnd, IDC_RESUME_BUTTON), NULL, buttonXPos, buttonYPos, 70, 30, SWP_NOZORDER);

    buttonYPos += 40;
    SetWindowPos(GetDlgItem(hwnd, IDC_TERMINATE_BUTTON), NULL, buttonXPos, buttonYPos, 70, 30, SWP_NOZORDER);

    int searchFieldXPos = buttonXPos - 70;
    int searchButtonXPos = buttonXPos;
    SetWindowPos(hwndEdit, NULL, searchFieldXPos, buttonYPos + 40, 150, 30, SWP_NOZORDER);
    SetWindowPos(GetDlgItem(hwnd, IDC_PROCESS_SEARCH_BUTTON), NULL, searchButtonXPos, buttonYPos + 60 + 15, 70, 30, SWP_NOZORDER);

    // Update the size of the ListView to fill the window
    SetWindowPos(listView, NULL, 0, 0, LOWORD(lParam) - 150, HIWORD(lParam), SWP_NOZORDER);

    return 0;
}




    case WM_NOTIFY:
    {
    switch (((LPNMHDR)lParam)->code)
    {
    case LVN_ITEMCHANGED:  // This notification code is sent to a ListView when an item changes
        NMLISTVIEW* pnmv = (NMLISTVIEW*)lParam;
        // Ensure this is the notification message we are interested in
        if (pnmv && ((pnmv->uOldState & LVIS_SELECTED) != (pnmv->uNewState & LVIS_SELECTED)))
        {
            int i = pnmv->iItem;
            BOOL selected = pnmv->uNewState & LVIS_SELECTED;
            if (selected)
            {
                // The item has just been selected, do something with it
                CHAR buffer[256];
                ListView_GetItemText(listView, i, 0, buffer, 256);
                selectedPID = atoi(buffer);
            }
            else
            {
                // The item has just been deselected, do something with it
                selectedPID = -1; // Reset to default value to indicate no process is selected
            }
        }
        break;
    }
    break;
        }

     case WM_DRAWITEM:
        {
            DRAWITEMSTRUCT* pDIS = (DRAWITEMSTRUCT*)lParam;
            if (pDIS->itemAction & ODA_SELECT) {
                COLORREF oldTextColor;
                if (pDIS->itemState & ODS_SELECTED) {
                    oldTextColor = SetTextColor(pDIS->hDC, RGB(255, 255, 255));
                    SetBkColor(pDIS->hDC, RGB(0, 0, 255));
                } else {
                    oldTextColor = SetTextColor(pDIS->hDC, RGB(0, 0, 0));
                    SetBkColor(pDIS->hDC, RGB(125, 30, 125));
                }
                // Override the default system behaviour to draw the text background
                ExtTextOut(pDIS->hDC, pDIS->rcItem.left, pDIS->rcItem.top, ETO_OPAQUE, &pDIS->rcItem, NULL, 0, NULL);
                // Draw the text
                DrawText(pDIS->hDC, (LPCSTR)pDIS->itemData, -1, &pDIS->rcItem, DT_CENTER);

                // Restore the old text color
                SetTextColor(pDIS->hDC, oldTextColor);
            }
            break;
        }

case WM_COMMAND:
{
    int wmId = LOWORD(wParam);

    switch (wmId) {
    case IDC_UPDATE_BUTTON:
        ListView_DeleteAllItems(listView);
        AddProcessesToListView(listView, "");
        break;
    case IDC_PAUSE_BUTTON:
        if (selectedPID != -1) {
            HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, selectedPID);
            if (hProcess) {
                pfnNtSuspendProcess(hProcess);
                CloseHandle(hProcess);
            }
        }
        break;
    case IDC_RESUME_BUTTON:
        if (selectedPID != -1) {
            HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, selectedPID);
            if (hProcess) {
                pfnNtResumeProcess(hProcess);
                CloseHandle(hProcess);
            }
        }
        break;
    case IDC_TERMINATE_BUTTON:
        if (selectedPID != -1) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, selectedPID);
            if (hProcess) {
                TerminateProcess(hProcess, 0);
                CloseHandle(hProcess);
            }
        }
        break;
    case IDC_PROCESS_SEARCH_BUTTON:
    {
        char searchQuery[256];
        GetWindowText(hwndEdit, searchQuery, sizeof(searchQuery));
        // Search function
        ListView_DeleteAllItems(listView);
        // Applies searchQuery
        AddProcessesToListView(listView, searchQuery);
    }
    break;
    default:
        break;
    }
    return 0;
}



    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

   return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, INT nShowCmd) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Could not register window class", "Error", MB_OK);
        return 0;
    }
    HWND hwnd = CreateWindowEx(0, CLASS_NAME, "Process Memory Monitor",
                               WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                               CW_USEDEFAULT, CW_USEDEFAULT,
                               CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
    if (!hwnd) {
        MessageBox(NULL, "Could not create window", "Error", MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);
    HMODULE hNtDll = GetModuleHandle("ntdll.dll");
    if (hNtDll) {
        pfnNtSuspendProcess = (NtSuspendProcess)GetProcAddress(hNtDll, "NtSuspendProcess");
        pfnNtResumeProcess = (NtResumeProcess)GetProcAddress(hNtDll, "NtResumeProcess");
    }
    if (!pfnNtSuspendProcess || !pfnNtResumeProcess) {
        MessageBox(NULL, "Could not load function pointers", "Error", MB_OK);
        return 0;
    }

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
