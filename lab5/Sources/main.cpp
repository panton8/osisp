#include <windows.h>
#include <commctrl.h>
#include <time.h>

#define ID_ANALYZE_BUTTON 1001
#define ID_LISTVIEW 1002

// Преобразование FILETIME в time_t
time_t FileTimeToTimeT(FILETIME* ft);

void AnalyzeRegistry(HWND hListView);

// Обработчик главного окна
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_ANALYZE_BUTTON:
            // Обработка нажатия кнопки "Анализировать"
            HWND hListView = GetDlgItem(hWnd, ID_LISTVIEW);
            AnalyzeRegistry(hListView);
            break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProcA(hWnd, message, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEXA wcex = { sizeof(WNDCLASSEXA) };
    wcex.lpfnWndProc = MainWindowProc;
    wcex.hInstance = hInstance;
    wcex.lpszClassName = "MainWindowClass";
    RegisterClassExA(&wcex);

    HWND hWnd = CreateWindowExA(
        0,
        "MainWindowClass",
        "Анализатор реестра",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hWnd == NULL)
        return 0;

    HWND hButton = CreateWindowA(
        "BUTTON", "Анализировать",
        WS_CHILD | WS_VISIBLE,
        10, 10, 110, 30,
        hWnd, (HMENU)ID_ANALYZE_BUTTON, hInstance, NULL
    );

    HWND hListView = CreateWindowExA(
        0,
        WC_LISTVIEWA, "Результаты",
        WS_CHILD | WS_VISIBLE | LVS_REPORT,
        10, 50, 760, 500,
        hWnd, (HMENU)ID_LISTVIEW, hInstance, NULL
    );

    LVCOLUMNA lvColumn = { 0 };
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH;
    lvColumn.cx = 200;
    lvColumn.pszText = "Запись";
    ListView_InsertColumn(hListView, 0, &lvColumn);

    lvColumn.pszText = "Статус";
    ListView_InsertColumn(hListView, 1, &lvColumn);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Цикл обработки сообщений
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}

void AnalyzeRegistry(HWND hListView)
{
    // Очистка представления списка
    ListView_DeleteAllItems(hListView);

    // Получение доступа к корневому ключу реестра
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Software", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD subkeyCount;
        DWORD maxSubkeySize;
        RegQueryInfoKeyA(hKey, NULL, NULL, NULL, &subkeyCount, &maxSubkeySize, NULL, NULL, NULL, NULL, NULL, NULL);

        // Установка временного порога для создания записи (в данном случае 12 месяцев)
        time_t treshold = time(NULL) - 12 * 30 * 24 * 60 * 60;

        // Перебор подключей реестра
        for (DWORD i = 0; i < subkeyCount; i++)
        {
            char subkeyName[MAX_PATH];
            DWORD subkeyNameSize = MAX_PATH;
            FILETIME ftLastWriteTime;
            if (RegEnumKeyExA(hKey, i, subkeyName, &subkeyNameSize, NULL, NULL, NULL, &ftLastWriteTime) == ERROR_SUCCESS)
            {
                // Анализ и проверка подключа реестра
                // Добавление информации о записи в представление списка
                LVITEMA lvItem = { 0 };
                lvItem.mask = LVIF_TEXT;
                lvItem.pszText = subkeyName;
                lvItem.iItem = i;
                ListView_InsertItem(hListView, &lvItem);

                // Проверка на устаревшую запись
                if (FileTimeToTimeT(&ftLastWriteTime) < treshold) {
                    ListView_SetItemText(hListView, i, 1, "Может быть удалено");
                }
            }
        }

        // Закрытие корневого ключа реестра
        RegCloseKey(hKey);
    }
}

time_t FileTimeToTimeT(FILETIME* ft)
{
    ULARGE_INTEGER ui;
    ui.LowPart = ft->dwLowDateTime;
    ui.HighPart = ft->dwHighDateTime;

    return (time_t)((ui.QuadPart - 116444736000000000ULL) / 10000000ULL);
}
