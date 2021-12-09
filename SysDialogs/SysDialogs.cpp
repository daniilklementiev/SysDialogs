// SysDialogs.cpp : 
#pragma comment(lib, "shlwapi.lib")
#include "framework.h"
#include "SysDialogs.h"
#include <commdlg.h>
#include <Shlwapi.h>
#include <stdio.h>

#define MAX_LOADSTRING          100
#define CMD_OPEN_FILE           1001
#define CMD_SAVE_FILE           1002

// Global Variables:
HINSTANCE   hInst;                                
WCHAR       szTitle[MAX_LOADSTRING];                  
WCHAR       szWindowClass[MAX_LOADSTRING];            
HWND        fNameStatic;
HWND        fEllipsis;
HWND        fSave;
HWND        editor;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
DWORD   CALLBACK    OpenFileClick(LPVOID);
DWORD   CALLBACK    SaveFileClick(LPVOID);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);


    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SYSDIALOGS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SYSDIALOGS));

    MSG msg;


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



ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SYSDIALOGS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SYSDIALOGS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        fEllipsis = CreateWindowExW(0, L"Button", L"...",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
            220, 10, 37, 23, hWnd, (HMENU)CMD_OPEN_FILE, hInst, 0);
        fNameStatic = CreateWindowExW(0, L"Edit", L"No file selected", 
            WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_READONLY, 
            10, 10, 200, 23, hWnd, 0, hInst, 0);
        editor = CreateWindowExW(0, L"Edit", L"",
            WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_MULTILINE | WS_BORDER,
            10, 60, 300, 300, hWnd, 0, hInst, 0);
        fSave = CreateWindowExW(0, L"Button", L"Save as...", 
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 
            220, 365, 75, 23, hWnd, (HMENU)CMD_SAVE_FILE, hInst, 0);

    }
        break;
    case WM_COMMAND: {
        int notifId = HIWORD(wParam);
        // Parse notifications:
        switch (notifId) {


        }
        int wmId = LOWORD(wParam);
        // Parse the menu selections
        switch (wmId) {
        case CMD_OPEN_FILE: {
            
            CreateThread(NULL, 0, OpenFileClick, &hWnd, 0, NULL);
            break;
        }
        case CMD_SAVE_FILE:
            SendMessageW(fSave, WM_KILLFOCUS, 0, 0);
            SaveFileClick(&hWnd);
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

/****************************************************************************************************************************************/
HANDLE file;
DWORD CALLBACK OpenFileClick(LPVOID params) {
    HWND hWnd = *((HWND*)params);
    SendMessageW(fEllipsis, WM_KILLFOCUS, 0, 0);
    WCHAR fname[512];

    fname[0] = '\0';
    OPENFILENAMEW ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFile = fname;
    ofn.nMaxFile = 512;
    ofn.nMaxFileTitle = sizeof(fname);
    if (GetOpenFileNameW(&ofn)) {
        SendMessageW(fNameStatic, WM_SETTEXT, 0, (LPARAM)ofn.lpstrFile);
        file = CreateFileW(fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        
        if (file == 0)
        {
            SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)L"File open error");
        }
        else {
            DWORD fSize;
            fSize = GetFileSize(file, NULL);
            if (fSize > 0)
            {
                char* content = new char[fSize + 1];
                DWORD read;
                if(ReadFile(file, content, fSize, &read, NULL)){
                    content[fSize] = '\0';
                    SendMessageA(editor, WM_SETTEXT, 0, (LPARAM)content);
                    delete[] content;
                }
                else {
                    SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)"File read error");
                }
            }
            else {
                SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)"File is empty");
            }
            CloseHandle(file);
        }
    }
    else {
        SendMessageW(fNameStatic, WM_SETTEXT, 0, (LPARAM)L"Selection cancelled");
    }
    
    

    return 0;
}

DWORD   CALLBACK    SaveFileClick(LPVOID params) {
    HWND            hWnd                = *((HWND*)params);
    WCHAR           fname[512];
    fname[0] = '\0';
    OPENFILENAMEW   ofn;
    
    ZeroMemory(&ofn, sizeof(ofn));


    char filterExt[][6] = { ".txt" };
    char cCustomFilter[256] = "\0\0";
    int nFilterIndex = 0;
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.hInstance = hInst;
    ofn.lpstrFilter = L"��������� ���������(*.txt)\0*.txt\0��� �����\0*.*\0\0";
    ofn.lpstrCustomFilter = (LPWSTR)cCustomFilter;
    ofn.nMaxCustFilter = 256;
    ofn.nFilterIndex = nFilterIndex;
    ofn.lpstrFile = fname;
    ofn.nMaxFile = 255;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = 0;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.nFileOffset = 0;
    ofn.nFileExtension = 0;
    ofn.lpstrDefExt = NULL;
    ofn.lCustData = NULL;
    ofn.lpfnHook = NULL;
    ofn.lpTemplateName = NULL;

    if (GetSaveFileName(&ofn))
    {
        DWORD fSize;
        fSize = GetFileSize(file, NULL);
        char* content = new char[fSize + 1];
        DWORD read;
        if(ReadFile(file, content, fSize, &read, NULL)) {
            SendMessageA(editor, WM_GETTEXT, 0, (LPARAM)content);
            if (file == 0)
            {
                SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)L"File open error");
            }
            else {
                
                if (fSize > 0)
                {
                    DWORD write;
                    

                    if (WriteFile(file, content, fSize, &write, NULL)) {
                        content[fSize] = '\0';
                        SendMessageA(editor, WM_SETTEXT, 0, (LPARAM)L"");
                        delete[] content;
                    }
                    else {
                        SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)L"File write error");
                    }
                }
                else {
                    SendMessageW(editor, WM_SETTEXT, 0, (LPARAM)"Error");
                }
                CloseHandle(file);
            }
        }


    }
    else {
        SendMessageW(fNameStatic, WM_SETTEXT, 0, (LPARAM)L"Selection cancelled");
    }
    return 0;
}