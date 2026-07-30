#ifndef DEMOS_H
#define DEMOS_H

#include "app.h"

void CycleDemoColor(AppState *app);
void ResetDemoColor(AppState *app);

void DrawDemo(
    HDC hdc,
    const AppState *app
);

#endif
