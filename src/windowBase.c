#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

const char g_szClassName[] = "myWindowClass";

enum ControlId
{
    ID_BUTTON_CYCLE = 1001,
    ID_BUTTON_RESET = 1002
};

typedef struct AppState
{
    HWND cycleButton;
    HWND resetButton;
    HWND demoSelector;
    
    RECT clientRect;
    RECT demoRect;
    RECT statusRect;
    
    POINT mousePosition;
    
    HBRUSH statusBrush;
    HPEN demoPen;
    HPEN statusPen;
    
    int colorIndex;
    int windowWidth;
    int windowHeight;
} AppState;

static void UpdateLayout(HWND hwnd, AppState *app)
{
    GetClientRect(hwnd, &app->clientRect);
    
    app->windowWidth = app->clientRect.right - app->clientRect.left;
    
    app->windowHeight = app->clientRect.bottom - app->clientRect.top;
    
    SetRect(
        &app->statusRect,
        0,
        app->clientRect.bottom - 20,
        app->clientRect.right,
        app->clientRect.bottom
    );
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static char printDimensions[64];
    static char windowInfo[128];
    
    static int redVal[4] = {245,245,66,66};
    static int greVal[4] = {66,245,245,66};
    static int bluVal[4] = {66,66,66,245};
    
    static AppState app;

    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *) lParam;
            HINSTANCE hInstance = pCreate->hInstance;

            app.demoPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            SetRect(&app.demoRect, 20, 20, 140, 100);

            app.statusPen = CreatePen(PS_SOLID, 3, RGB(0, 100, 0));
            app.statusBrush = CreateSolidBrush(RGB(190, 100, 30));
            UpdateLayout(hwnd, &app);
            
            app.cycleButton = CreateWindow(
                "BUTTON",
                "Cycle",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                50, 150, 100, 30,
                hwnd,
                (HMENU)ID_BUTTON_CYCLE,
                hInstance,
                NULL
            );
            
            app.resetButton = CreateWindow(
                "BUTTON",
                "Reset",
                WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                175, 150, 100, 30,
                hwnd,
                (HMENU)ID_BUTTON_RESET,
                hInstance,
                NULL
            );
        }
        break;
        
        case WM_COMMAND: {
            int control_id = LOWORD(wParam);
            int notification = HIWORD(wParam);
            
            if (notification == BN_CLICKED) {
                //MessageBeep(MB_OK);
                if(control_id == ID_BUTTON_CYCLE){
                    app.colorIndex++;
                    app.colorIndex = app.colorIndex % (sizeof(redVal) / sizeof(redVal[0]));
                }
                else if(control_id == ID_BUTTON_RESET){
                    app.colorIndex = 0;
                }
                
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
            break;
        }
        

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &app.clientRect);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, app.clientRect.right, app.clientRect.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            // Clear background in memory DC
            FillRect(memDC, &app.clientRect, (HBRUSH)(COLOR_WINDOW + 1));

            // Draw static rectangle
            SelectObject(memDC, app.demoPen);
            HBRUSH currentBrush = CreateSolidBrush(RGB(redVal[app.colorIndex], greVal[app.colorIndex], bluVal[app.colorIndex]));
            
            HBRUSH oldBrush = SelectObject(memDC, currentBrush);
            
            Rectangle(memDC, app.demoRect.left, app.demoRect.top, app.demoRect.right, app.demoRect.bottom);
            
            SelectObject(memDC, oldBrush);
            
            DeleteObject(currentBrush);

            SetBkMode(memDC, TRANSPARENT);
            sprintf(printDimensions, " Mouse X: %ld\n Mouse Y: %ld", app.mousePosition.x, app.mousePosition.y);
            DrawText(memDC, printDimensions, -1, &app.demoRect, DT_LEFT);

            // Draw bottom bar rectangle
            SelectObject(memDC, app.statusPen);
            SelectObject(memDC, app.statusBrush);
            Rectangle(memDC, app.statusRect.left, app.statusRect.top, app.statusRect.right, app.statusRect.bottom);

            sprintf(windowInfo, "Window width: %d    Window height: %d", app.windowWidth, app.windowHeight);
            DrawText(memDC, windowInfo, -1, &app.statusRect, DT_LEFT);

            // Final blit
            BitBlt(hdc, 0, 0, app.clientRect.right, app.clientRect.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_MOUSEMOVE:
        {
            app.mousePosition.x = (LONG)LOWORD(lParam);
            app.mousePosition.y = (LONG)HIWORD(lParam);
            
            InvalidateRect(hwnd, &app.demoRect, FALSE);
        }
        break;

        case WM_SIZE:
        {
            UpdateLayout(hwnd, &app);
            InvalidateRect(hwnd, NULL, FALSE);
        }
        break;

        case WM_ERASEBKGND:
            return 1; // Prevent flicker

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            if (app.demoPen != NULL)
            {
                DeleteObject(app.demoPen);
                app.demoPen = NULL;
            }
            if (app.statusPen != NULL)
            {
                DeleteObject(app.statusPen);
                app.statusPen = NULL;
            }
            if (app.statusBrush != NULL)
            {
                DeleteObject(app.statusBrush);
                app.statusBrush = NULL;
            }
            
            PostQuitMessage(0);
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    hwnd = CreateWindowEx(
        WS_EX_COMPOSITED | WS_EX_CLIENTEDGE,
        g_szClassName,
        "Interfacing in Memory Safe Hues",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    while (GetMessage(&Msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    return Msg.wParam;
}
