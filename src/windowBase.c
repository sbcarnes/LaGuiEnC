#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"

const char g_szClassName[] = "myWindowClass";

static const COLORREF demoColors[] = 
{
    RGB(245, 66, 66),
    RGB(245, 245, 66),
    RGB(66, 245, 66),
    RGB(66, 66, 245)
};

void UpdateLayout(HWND hwnd, AppState *app)
{
    GetClientRect(hwnd, &app->clientRect);
    
    app->windowWidth = app->clientRect.right - app->clientRect.left;
    
    app->windowHeight = app->clientRect.bottom - app->clientRect.top;
    
    SetRect(
        &app->demoRect,
        UI_MARGIN,
        65,
        UI_MARGIN + DEMO_WIDTH,
        65 + DEMO_HEIGHT
    );
    
    SetRect(
        &app->statusRect,
        0,
        app->clientRect.bottom - STATUS_HEIGHT,
        app->clientRect.right,
        app->clientRect.bottom
    );
    
    if (app->demoSelector != NULL)
    {
        MoveWindow(
            app->demoSelector,
            UI_MARGIN,
            UI_MARGIN,
            SELECTOR_WIDTH,
            200,
            TRUE
        );
    }
    
    if (app->cycleButton != NULL)
    {
        MoveWindow(
            app->cycleButton,
            UI_MARGIN,
            app->demoRect.bottom + UI_GAP,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            TRUE
        );
    }
    
    if (app->resetButton != NULL)
    {
        MoveWindow(
            app->resetButton,
            UI_MARGIN + BUTTON_WIDTH + UI_GAP,
            app->demoRect.bottom + UI_GAP,
            BUTTON_WIDTH,
            BUTTON_HEIGHT,
            TRUE
        );
    }
}

void DestroyAppResources(AppState *app)
{
    if (app->demoPen != NULL)
    {
        DeleteObject(app->demoPen);
        app->demoPen = NULL;
    }
    
    if (app->statusPen != NULL)
    {
        DeleteObject(app->statusPen);
        app->statusPen = NULL;
    }
    
    if (app->statusBrush != NULL)
    {
        DeleteObject(app->statusBrush);
        app->statusBrush = NULL;
    }
}

BOOL CreateAppResources(AppState *app)
{
    app->demoPen = CreatePen(
        PS_SOLID,
        2,
        RGB(0, 0, 0)
    );
    
    app->statusPen = CreatePen(
        PS_SOLID,
        3,
        RGB(0, 100, 0)
    );
    
    app->statusBrush = CreateSolidBrush(
        RGB(190, 100, 30)
    );
    
    if (app->demoPen == NULL ||
        app->statusPen == NULL ||
        app->statusBrush == NULL)
    {
        DestroyAppResources(app);
        return FALSE;
    }
    
    return TRUE;
}

BOOL CreateAppControls(HWND hwnd, HINSTANCE hInstance, AppState *app)
{
    app->cycleButton = CreateWindow(
        "BUTTON",
        "Cycle",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        50, 150, 100, 30,
        hwnd,
        (HMENU)ID_BUTTON_CYCLE,
        hInstance,
        NULL
    );
    
    app->resetButton = CreateWindow(
        "BUTTON",
        "Reset",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
        175, 150, 100, 30,
        hwnd,
        (HMENU)ID_BUTTON_RESET,
        hInstance,
        NULL
    );
    
    app->demoSelector = CreateWindow(
        "COMBOBOX",
        "",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | WS_VSCROLL,
        20, 110, 255, 200,
        hwnd,
        (HMENU)ID_COMBO_DEMO_SELECTOR,
        hInstance,
        NULL
    );
    
    SendMessage(
        app->demoSelector,
        CB_ADDSTRING,
        0,
        (LPARAM)"Color Cycling"
    );
    
    SendMessage(
        app->demoSelector,
        CB_ADDSTRING,
        0,
        (LPARAM)"Mouse Tracking"
    );
    
    SendMessage(
        app->demoSelector,
        CB_ADDSTRING,
        0,
        (LPARAM)"Keyboard Input and Focus"
    );
    
    SendMessage(
        app->demoSelector,
        CB_SETCURSEL,
        DEMO_COLOR_CYCLE,
        0
    );
    
    app->currentDemo = DEMO_COLOR_CYCLE;
    
    if (app->cycleButton == NULL ||
        app->resetButton == NULL ||
        app->demoSelector == NULL)
    {
        return FALSE;
    }
    
    return TRUE;
}

void DrawStatusBar(HDC hdc, const AppState *app)
{
    char windowInfo[128];
    
    HPEN oldPen = (HPEN)SelectObject(hdc, app->statusPen);
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, app->statusBrush);
    
    Rectangle(
        hdc,
        app->statusRect.left,
        app->statusRect.top,
        app->statusRect.right,
        app->statusRect.bottom
    );
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    
    snprintf(
        windowInfo,
        sizeof(windowInfo),
        "Window width: %d    Window height: %d",
        app->windowWidth,
        app->windowHeight
    );
    
    RECT textRect = app->statusRect;
    
    int oldBackgroundMode = SetBkMode(hdc, TRANSPARENT);
    
    DrawText(
        hdc, windowInfo, -1, &textRect, DT_LEFT
    );
    
    SetBkMode(hdc, oldBackgroundMode);
}

static void DrawColorDemo(HDC hdc, const AppState *app)
{
    HBRUSH colorBrush = CreateSolidBrush(demoColors[app->colorIndex]);
    
    if (colorBrush == NULL)
    {
        return;
    }
    
    HPEN oldPen = (HPEN)SelectObject(hdc, app->demoPen);
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, colorBrush);
    
    Rectangle(
        hdc,
        app->demoRect.left,
        app->demoRect.top,
        app->demoRect.right,
        app->demoRect.bottom
    );
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    
    DeleteObject(colorBrush);
}

static void DrawMouseDemo(HDC hdc, const AppState *app)
{
    char mouseText[64];
    
    HPEN oldPen = (HPEN)SelectObject(hdc, app->demoPen);
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    
    Rectangle(
        hdc,
        app->demoRect.left,
        app->demoRect.top,
        app->demoRect.right,
        app->demoRect.bottom
    );
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    
    snprintf(
        mouseText,
        sizeof(mouseText),
        "Mouse X: %ld\nMouse Y: %ld",
        app->mousePosition.x,
        app->mousePosition.y
    );
    
    RECT textRect = app->demoRect;
    
    /*
        Give padding to the top-left corner of the rectangle
    */
    
    textRect.left += 10;
    textRect.top += 10;
    
    int oldBackgroundMode = SetBkMode(hdc, TRANSPARENT);
    
    DrawText(
        hdc, mouseText, -1, &textRect, DT_LEFT
    );
    
    SetBkMode(hdc, oldBackgroundMode);
}

static void DrawKeyboardDemo(HDC hdc, const AppState *app)
{
    char keyboardText[128];
    
    HPEN oldPen = (HPEN)SelectObject(hdc, app->demoPen);
    
    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(WHITE_BRUSH));
    
    Rectangle(
        hdc,
        app->demoRect.left,
        app->demoRect.top,
        app->demoRect.right,
        app->demoRect.bottom
    );
    
    SelectObject(hdc, oldBrush);
    SelectObject(hdc, oldPen);
    
    if (app->lastVirtualKey == 0)
    {
        snprintf(
            keyboardText,
            sizeof(keyboardText),
            "Keyboard focus: %s\n"
            "Key state: No key recorded\n"
            "Virtual key: None",
            app->hasKeyboardFocus ? "Yes" : "No"
        );
    }
    else
    {
        snprintf(
            keyboardText,
            sizeof(keyboardText),
            "Keyboard focus: %s\n"
            "Key state: %s\n"
            "Virtual key: %u",
            app->hasKeyboardFocus ? "Yes" : "No",
            app->keyIsDown ? "Down" : "Up",
            app->lastVirtualKey
        );
    }
    
    RECT textRect = app->demoRect;
    
    textRect.left += 10;
    textRect.top += 10;
    
    int oldBackgroundMode = SetBkMode(hdc, TRANSPARENT);
    
    DrawText(
        hdc, keyboardText, -1, &textRect, DT_LEFT
    );
    
    SetBkMode(hdc, oldBackgroundMode);
}

static void DrawDemo(HDC hdc, const AppState *app)
{
    
    switch (app->currentDemo)
    {
        case DEMO_COLOR_CYCLE:
            DrawColorDemo(hdc, app);
            break;
            
        case DEMO_MOUSE_TRACKING:
            DrawMouseDemo(hdc, app);
            break;
        
        case DEMO_KEYBOARD_INPUT:
            DrawKeyboardDemo(hdc, app);
            break;
        
        default:
            break;
    }
}

void UpdateDemoControls(AppState *app)
{
    BOOL showColorControls = app->currentDemo == DEMO_COLOR_CYCLE;
    
    ShowWindow(app->cycleButton, showColorControls ? SW_SHOW : SW_HIDE);
    
    ShowWindow(app->resetButton, showColorControls ? SW_SHOW : SW_HIDE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static AppState app;

    switch (msg)
    {
        case WM_CREATE:
        {
            CREATESTRUCT *pCreate = (CREATESTRUCT *) lParam;
            HINSTANCE hInstance = pCreate->hInstance;

            if (!CreateAppResources(&app))
            {
                return -1;
            }
            
            if (!CreateAppControls(hwnd, hInstance, &app))
            {
                DestroyAppResources(&app);
                return -1;
            }

            
            UpdateLayout(hwnd, &app);
            UpdateDemoControls(&app);
            
            
            
        }
        break;
        
        case WM_COMMAND: {
            int control_id = LOWORD(wParam);
            int notification = HIWORD(wParam);
            
            if (notification == BN_CLICKED) {
                //MessageBeep(MB_OK);
                if(control_id == ID_BUTTON_CYCLE){
                    app.colorIndex = 
                        (app.colorIndex + 1) % 
                        (sizeof(demoColors) / sizeof(demoColors[0]));
                }
                else if(control_id == ID_BUTTON_RESET){
                    app.colorIndex = 0;
                }
                
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
            
            if (control_id == ID_COMBO_DEMO_SELECTOR && notification == CBN_SELCHANGE)
            {
                LRESULT selection = SendMessage(
                    app.demoSelector, CB_GETCURSEL, 0, 0
                );
                
                if (selection != CB_ERR)
                {
                    app.currentDemo = (DemoId)selection;
                    
                    UpdateDemoControls(&app);
                    
                    if (app.currentDemo == DEMO_KEYBOARD_INPUT)
                    {
                        SetFocus(hwnd);
                    }
                    
                    InvalidateRect(hwnd, &app.demoRect, FALSE);
                }
            }
            break;
        }
        
        case WM_LBUTTONDOWN:
        {
            if (app.currentDemo == DEMO_KEYBOARD_INPUT)
            {
                SetFocus(hwnd);
            }
        }
        break;

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

            DrawDemo(memDC, &app);
            DrawStatusBar(memDC, &app);

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
            
            if (app.currentDemo == DEMO_MOUSE_TRACKING)
            {
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
            
        }
        break;
        
        case WM_KEYDOWN:
        {
            app.lastVirtualKey = (UINT)wParam;
            app.keyIsDown = TRUE;
            
            if (app.currentDemo == DEMO_KEYBOARD_INPUT)
            {
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
        }
        break;
        
        case WM_KEYUP:
        {
            app.lastVirtualKey = (UINT)wParam;
            app.keyIsDown = FALSE;
            
            if (app.currentDemo == DEMO_KEYBOARD_INPUT)
            {
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
        }
        break;
        
        case WM_SETFOCUS:
        {
            app.hasKeyboardFocus = TRUE;
            
            if (app.currentDemo == DEMO_KEYBOARD_INPUT)
            {
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
        }
        break;
        
        case WM_KILLFOCUS:
        {
            app.hasKeyboardFocus = FALSE;
            
            if (app.currentDemo == DEMO_KEYBOARD_INPUT)
            {
                InvalidateRect(hwnd, &app.demoRect, FALSE);
            }
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
        {
            DestroyAppResources(&app);
            PostQuitMessage(0);
        }
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
