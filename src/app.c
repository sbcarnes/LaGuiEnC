#include "app.h"

#include <stdio.h>

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

void UpdateDemoControls(AppState *app)
{
    BOOL showColorControls = app->currentDemo == DEMO_COLOR_CYCLE;
    
    ShowWindow(app->cycleButton, showColorControls ? SW_SHOW : SW_HIDE);
    
    ShowWindow(app->resetButton, showColorControls ? SW_SHOW : SW_HIDE);
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