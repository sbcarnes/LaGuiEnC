#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "app.h"
#include "demos.h"

const char g_szClassName[] = "myWindowClass";

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
                    CycleDemoColor(&app);
                }
                else if(control_id == ID_BUTTON_RESET){
                    ResetDemoColor(&app);
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
