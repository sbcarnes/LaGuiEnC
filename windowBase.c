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
    
    static HBRUSH bkgnd_01;  // Handle of background-color brush
    static HBRUSH bkgnd_02;  // Handle of background-color brush
    static HPEN hPen;        // Handle for display box outline
    
    static HINSTANCE hInstance;
    
    static POINT pt;         // x and y coordinates of cursor
    
    static char printDimensions[64];
    static int windowWidth;
    static int windowHeight;
    
    
	switch(msg)
	{
		case WM_CREATE:
            
            hdc = GetDC(hwnd);
            GetClientRect(hwnd, &rcClient);
            windowWidth = rcClient.right - rcClient.left;
            windowHeight = rcClient.bottom - rcClient.top;
            sprintf(printDimensions, "Width: %d \n\n Height: %d", windowWidth, windowHeight);
            hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            SetRect(&hzBox, 20, 20, 140, 100); 
            
        break;
        case WM_LBUTTONDOWN:
		{
            
            printf("Click");
            
		}
		break;
        case WM_PAINT:
            {
                BeginPaint(hwnd, &ps);
                
                SelectObject(ps.hdc, CreateSolidBrush(RGB(177, 177, 230)));
                
                // Accidentally only set the hdc instead of ps.hdc
                Rectangle(ps.hdc, hzBox.left, hzBox.top, 
                hzBox.right, hzBox.bottom);
                SetBkMode(ps.hdc, TRANSPARENT);
                DrawText(ps.hdc, printDimensions, 32, &hzBox, DT_LEFT);
                memset(printDimensions, '\0', sizeof(printDimensions));
                EndPaint(hwnd, &ps);
            }
        break;
        case WM_SIZING:
            
        break;
        case WM_MOUSEMOVE:
            pt.x = (LONG) LOWORD(lParam); 
            pt.y = (LONG) HIWORD(lParam);
            hdc = GetDC(hwnd);
            sprintf(printDimensions, " Mouse X: %ld \n\n Mouse Y: %ld", pt.x, pt.y);
            SelectObject(hdc, CreateSolidBrush(RGB(177, 177, 230)));
            Rectangle(hdc, hzBox.left, hzBox.top, 
                hzBox.right, hzBox.bottom);
            SetBkMode(hdc, TRANSPARENT);
            DrawText(hdc, printDimensions, 32, &hzBox, DT_LEFT);
            memset(printDimensions, '\0', sizeof(printDimensions));
            ReleaseDC(hwnd, hdc);
            
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
