// SystemMetrix.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "SystemMetrix.h"

#define MAX_LOADSTRING 100
#define CMD_BUTTON_1 1001

struct Metrics {
    int nIndex;
    WCHAR indexConst[32];
    WCHAR description[1024];
};

Metrics metrics[] = {
    {56, L"SM_ARRANGE", L"Способ упорядочивания свернутых окон"},
    {67, L"SM_CLEANBOOT",  L"Режим загрузки системы"},
    {5, L"SM_CXBORDER",  L"Размер горизонтальной части окна в пикселях"},
    {6, L"SM_CYBORDER",  L"Размер веритикальной части окна в пикселях"},
    {2, L"SM_CXVSCROLL",  L"Ширина вертикальной полосы прокрутки в пикселях"},
    {20, L"SM_CYVSCROLL",  L"Высота рисунка 'стрелки' вертикальной полосы прокрутки в пикселях"},
    {1, L"SM_CYSCREEN",  L"Высота экрана основного монитора в пикселях"},
    {0, L"SM_CXSCREEN",  L"ширина экрана основного монитора в пикселях"},
};

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HWND mlist, mdescr, mval; 
ATOM centredWinAtom;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD CALLBACK      CreateControls(LPVOID);
DWORD CALLBACK      ChangeMetricsInfo(LPVOID);
DWORD CALLBACK       CreateNewWindow(LPVOID );
LRESULT CALLBACK    CentredWndProc(HWND, UINT, WPARAM, LPARAM);
void                StartInThread();


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SYSTEMMETRIX, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYSTEMMETRIX));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SYSTEMMETRIX));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SYSTEMMETRIX);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        CreateControls(&hWnd); 
        break;
    case WM_COMMAND: {

 
        int wmId = LOWORD(wParam);
        int notificationCode = HIWORD(wParam);
        if (notificationCode == CBN_SELCHANGE) {
            StartInThread();

            break;
        }
            
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case CMD_BUTTON_1: 
                CreateNewWindow(&hWnd);
                break;
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }

    }


        
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:

      
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


DWORD CALLBACK CreateControls(LPVOID params) {
    centredWinAtom = 0;
    HWND hWnd = *((HWND*)params);
    mlist = CreateWindowExW(0, L"Combobox", L"", 
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
        10, 10, 150, 300, hWnd, 0 , hInst, NULL);
    int mSize = sizeof(metrics) / sizeof(Metrics);
    for (size_t i = 0; i < mSize; i++) {
        SendMessageW(mlist, CB_ADDSTRING, 0, (LPARAM)metrics[i].indexConst);
        
    }
    mdescr = CreateWindowExW(0, L"Edit", L"",
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY,
        10, 40, 150, 200, hWnd, 0, hInst, NULL);

    mval = CreateWindowExW(0, L"Static", L"--",
        WS_CHILD | WS_VISIBLE,
        175, 10, 50, 15, hWnd, 0, hInst, NULL);

    CreateWindowExW(0, L"Button", L"New Centred Win",
        WS_CHILD | WS_VISIBLE,
        170, 150, 150, 25, hWnd, (HMENU)CMD_BUTTON_1, hInst, NULL);
   
       
  
    return 0;
}


DWORD CALLBACK ChangeMetricsInfo(LPVOID params) {
   
        int selectedIndex = SendMessageW(mlist, CB_GETCURSEL, 0, 0);
        SendMessageW(mdescr, WM_SETTEXT, 0, (LPARAM)metrics[selectedIndex].description);
        WCHAR str[12];
        _itow_s(GetSystemMetrics(metrics[selectedIndex].nIndex), str, 10);
        SendMessageW(mval, WM_SETTEXT, 0, (LPARAM)str);

      
    return 0;
}

DWORD CALLBACK CreateNewWindow(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    if (centredWinAtom == 0) {
        WNDCLASSEX wcl;
        ZeroMemory(&wcl, sizeof(wcl));
        wcl.cbSize = sizeof(wcl);
        wcl.lpfnWndProc = CentredWndProc;
        wcl.lpszClassName = L"CentredWindow";
        wcl.hbrBackground = (HBRUSH)5;
        wcl.hInstance = hInst;
        wcl.hCursor = LoadCursor(NULL, IDC_APPSTARTING);

        centredWinAtom = RegisterClassExW(&wcl);

    }
        
    int screenW = 0;
    int screenH = 0;



    HWND centWin = CreateWindowExW(0, L"CentredWindow", L"Centred", WS_OVERLAPPEDWINDOW, 0,0,screenH / 4, screenH / 4, hWnd, 0 , hInst, NULL);

    ShowWindow(centWin, SW_NORMAL);
    UpdateWindow(centWin);


    return 0;
}

LRESULT CALLBACK CentredWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
   
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
    


void StartInThread()
{

    CreateThread(NULL, 0, ChangeMetricsInfo, NULL, 0, NULL);
    
    
}
