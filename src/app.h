#ifndef APP_H
#define APP_H

#include <windows.h>

#define UI_MARGIN       20
#define UI_GAP          12
#define SELECTOR_WIDTH  300
#define DEMO_WIDTH      400
#define DEMO_HEIGHT     220
#define BUTTON_WIDTH    100
#define BUTTON_HEIGHT   30
#define STATUS_HEIGHT   24

enum ControlId
{
    ID_BUTTON_CYCLE = 1001,
    ID_BUTTON_RESET,
    ID_COMBO_DEMO_SELECTOR
};

typedef enum DemoId
{
    DEMO_COLOR_CYCLE = 0,
    DEMO_MOUSE_TRACKING,
    DEMO_KEYBOARD_INPUT
} DemoId;

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
    
    DemoId currentDemo;
    
    UINT lastVirtualKey;
    BOOL keyIsDown;
    BOOL hasKeyboardFocus;
} AppState;

void UpdateLayout(HWND hwnd, AppState *app);

BOOL CreateAppResources(AppState *app);
void DestroyAppResources(AppState *app);

BOOL CreateAppControls(
    HWND hwnd,
    HINSTANCE hInstance,
    AppState *app
);

void UpdateDemoControls(AppState *app);

void DrawStatusBar(
    HDC hdc,
    const AppState *app
);

#endif
