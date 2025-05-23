#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

const char g_szClassName[] = "myWindowClass";

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdcCompat;
    static HBITMAP hbmp;

    static RECT rcClient;
    static RECT hzBox;
    static RECT statDisplay;
    static RECT cnvRect;

    static HBRUSH csrTrkBr;
    static HBRUSH wnDimBr;
    static HPEN hPen;
    static HPEN statPen;

    static POINT pt;

    static char printDimensions[64];
    static char windowInfo[128];
    static int windowWidth;
    static int windowHeight;

    switch (msg)
    {
        case WM_CREATE:
        {
            HDC hdc = GetDC(hwnd);
            hdcCompat = CreateCompatibleDC(hdc);
            GetClientRect(hwnd, &rcClient);

            GetClientRect(hwnd, &cnvRect);

            hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            csrTrkBr = CreateSolidBrush(RGB(177, 177, 230));
            SetRect(&hzBox, 20, 20, 140, 100);

            windowWidth = rcClient.right - rcClient.left;
            windowHeight = rcClient.bottom - rcClient.top;

            statPen = CreatePen(PS_SOLID, 3, RGB(0, 100, 0));
            wnDimBr = CreateSolidBrush(RGB(190, 100, 30));
            SetRect(&statDisplay, 0, rcClient.bottom - 20, rcClient.right, rcClient.bottom);

            ReleaseDC(hwnd, hdc);
        }
        break;

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            GetClientRect(hwnd, &rcClient);

            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
            HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

            // Clear background in memory DC
            FillRect(memDC, &rcClient, (HBRUSH)(COLOR_WINDOW + 1));

            // Draw static rectangle
            SelectObject(memDC, hPen);
            SelectObject(memDC, csrTrkBr);
            Rectangle(memDC, hzBox.left, hzBox.top, hzBox.right, hzBox.bottom);

            SetBkMode(memDC, TRANSPARENT);
            sprintf(printDimensions, " Mouse X: %ld\n Mouse Y: %ld", pt.x, pt.y);
            DrawText(memDC, printDimensions, -1, &hzBox, DT_LEFT);

            // Draw bottom bar rectangle
            SelectObject(memDC, statPen);
            SelectObject(memDC, wnDimBr);
            Rectangle(memDC, statDisplay.left, statDisplay.top, statDisplay.right, statDisplay.bottom);

            sprintf(windowInfo, "Window width: %d    Window height: %d", windowWidth, windowHeight);
            DrawText(memDC, windowInfo, -1, &statDisplay, DT_LEFT);

            // Final blit
            BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, memDC, 0, 0, SRCCOPY);

            SelectObject(memDC, oldBitmap);
            DeleteObject(memBitmap);
            DeleteDC(memDC);

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_MOUSEMOVE:
        {
            pt.x = (LONG)LOWORD(lParam);
            pt.y = (LONG)HIWORD(lParam);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

        case WM_SIZING:
        case WM_SIZE:
        {
            GetClientRect(hwnd, &rcClient);
            windowWidth = rcClient.right - rcClient.left;
            windowHeight = rcClient.bottom - rcClient.top;
            SetRect(&statDisplay, 0, rcClient.bottom - 20, rcClient.right, rcClient.bottom);
            InvalidateRect(hwnd, NULL, TRUE);
        }
        break;

        case WM_ERASEBKGND:
            return 1; // Prevent flicker

        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;

        case WM_DESTROY:
            DeleteDC(hdcCompat);
            DeleteObject(hPen);
            DeleteObject(csrTrkBr);
            DeleteObject(statPen);
            DeleteObject(wnDimBr);
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
        WS_EX_CLIENTEDGE,
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
