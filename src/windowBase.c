#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

const char g_szClassName[] = "myWindowClass";

HBRUSH g_hbrBackground;
COLORREF g_rgbBackground = RGB(255, 255, 255);

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;          // paint data for BeginPaint and EndPaint
    
    static RECT rcClient;    // client-area rectangle
    static RECT hzBox;       // Horizontal box
    static RECT statDisplay; // Dynamic rectangle for holding window information
    static RECT wnRect;
    static RECT cnvRect;
    
    static HRGN hRgn;
    
    static HBRUSH csrTrkBr;  // Handle of cursor tracker brush
    static HBRUSH wnDimBr;   // Handle of window dimension brush
    static HBRUSH bkBr;
    static HPEN hPen;        // Handle for display box outline
    static HPEN statPen;
    
    static HINSTANCE hInstance;
    
    static POINT pt;         // x and y coordinates of cursor
    
    static char printDimensions[64];
    static char windowInfo[128];
    static int windowWidth;
    static int windowHeight;
    
    
	switch(msg)
	{
		case WM_CREATE:
            // Capture initial window information
            hdc = GetDC(hwnd);
            GetClientRect(hwnd, &rcClient);
            
            // Set up canvas rectangle
            GetClientRect(hwnd, &cnvRect);
            bkBr = (HBRUSH) GetBkColor(hdc);
            
            // Capture initial cursor coordinates
            pt.x = (LONG) LOWORD(lParam); 
            pt.y = (LONG) HIWORD(lParam);
            
            // Cursor tracking rectangle and readout
            hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            csrTrkBr = CreateSolidBrush(RGB(177, 177, 230));
            SetRect(&hzBox, 20, 20, 140, 100);
            
            // Capture initial window dimensions
            windowWidth = rcClient.right - rcClient.left;
            windowHeight = rcClient.bottom - rcClient.top;
            
            // Window dimension rectangle and readout
            statPen = CreatePen(PS_SOLID, 3, RGB(0, 100, 0));
            wnDimBr = CreateSolidBrush(RGB(190, 100, 30));
            SetRect(&statDisplay, 0, rcClient.bottom - 20, rcClient.right, rcClient.bottom);
            
        break;
        case WM_LBUTTONDOWN:
		{
            
            printf("Click");
            
		}
		break;
        case WM_PAINT:
            {
                //InvalidateRect(hwnd, NULL, TRUE);
                BeginPaint(hwnd, &ps);
                
                SelectObject(ps.hdc, bkBr);
                Rectangle(ps.hdc, cnvRect.left, cnvRect.top,
                cnvRect.right, cnvRect.bottom);
                
                // Paint the cursor tracker
                SelectObject(ps.hdc, hPen);
                SelectObject(ps.hdc, csrTrkBr);
                Rectangle(ps.hdc, hzBox.left, hzBox.top, 
                hzBox.right, hzBox.bottom);
                SetBkMode(ps.hdc, TRANSPARENT);
                sprintf(printDimensions, " Mouse X: %ld \n\n Mouse Y: %ld", pt.x, pt.y);
                DrawText(ps.hdc, printDimensions, 32, &hzBox, DT_LEFT);
                memset(printDimensions, '\0', sizeof(printDimensions));
                
                // Paint the window dimension display
                SelectObject(ps.hdc, statPen);
                SelectObject(ps.hdc, wnDimBr);
                Rectangle(ps.hdc, statDisplay.left, statDisplay.top, 
                statDisplay.right, statDisplay.bottom);
                SetBkMode(ps.hdc, TRANSPARENT);
                sprintf(windowInfo, "Window width: %d \t\t Window height: %d", windowWidth, windowHeight);
                DrawText(ps.hdc, windowInfo, 64, &statDisplay, DT_LEFT);
                memset(windowInfo, '\0', sizeof(windowInfo));
                
                EndPaint(hwnd, &ps);
            }
        break;
        case WM_SIZING:
            
            hdc = GetDC(hwnd);
            GetClientRect(hwnd, &rcClient);
            GetClientRect(hwnd, &cnvRect);
            InvalidateRect(hwnd, &wnRect, TRUE);
            windowWidth = rcClient.right - rcClient.left;
            windowHeight = rcClient.bottom - rcClient.top;
            SetRect(&statDisplay, 0, rcClient.bottom - 20, rcClient.right, rcClient.bottom);
            hRgn = CreateRectRgn(rcClient.left, rcClient.top,
            rcClient.right, rcClient.bottom);
            RedrawWindow(hwnd, &rcClient, hRgn, RDW_INVALIDATE | RDW_UPDATENOW);
            ReleaseDC(hwnd, hdc);
            
        break;
        case WM_MOUSEMOVE:
            
            hdc = GetDC(hwnd);
            GetClientRect(hwnd, &rcClient);
            GetClientRect(hwnd, &cnvRect);
            pt.x = (LONG) LOWORD(lParam); 
            pt.y = (LONG) HIWORD(lParam);
            hRgn = CreateRectRgn(rcClient.left, rcClient.top,
            rcClient.right, rcClient.bottom);
            RedrawWindow(hwnd, &rcClient, hRgn, RDW_INVALIDATE | RDW_UPDATENOW);
            ReleaseDC(hwnd, hdc);
            
        break;
        case WM_ERASEBKGND:
        break;
		case WM_CLOSE:
			DestroyWindow(hwnd);
		break;
		case WM_DESTROY:
            PostQuitMessage(0);
		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wc;
	HWND hwnd;
	MSG Msg;

	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.style		 = 0;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = hInstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = g_szClassName;
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		g_szClassName,
		"Interfacing in Memory Safe Hues",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
		NULL, NULL, hInstance, NULL);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}
	return Msg.wParam;
}
