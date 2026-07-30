#include "demos.h"

#include <stdio.h>

static const COLORREF demoColors[] = 
{
    RGB(245, 66, 66),
    RGB(245, 245, 66),
    RGB(66, 245, 66),
    RGB(66, 66, 245)
};

void CycleDemoColor(AppState *app)
{
    app->colorIndex =
        (app->colorIndex + 1) %
        (sizeof(demoColors) / sizeof(demoColors[0]));
}

void ResetDemoColor(AppState *app)
{
    app->colorIndex = 0;
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

void DrawDemo(
    HDC hdc,
    const AppState *app
)
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