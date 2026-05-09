// make use of rectangles functions when refactoring the case functions
// if this program becomes multithreaded, fix all static members

// todo: fix keybinds

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef WINDOW_PROCEDURE_HPP
#define WINDOW_PROCEDURE_HPP

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#pragma once


#include "rectangle.hpp"
#include "windows.hpp"
#include "errors.hpp"

#include <windows.h>
#include <functional>
#include <map>

// undocumented
void updatePreviousWindowDimensions(
    int newWidth, 
    int newHeight, 
    WindowProcedureData& windowProcedureData
) {

if (newWidth <= 0 || newHeight <= 0) {
    logError(ErrorType::invalidDimension);
    return;
}

    windowProcedureData.previousWindowWidth = newWidth;
    windowProcedureData.previousWindowHeight = newHeight;
}

// undocumented
void scaleRectangles(double integerScale, Window* window) {

if (!window) {
    logError(ErrorType::missingWindow);
    return;
}

if (!window -> rectangles) {
    logError(ErrorType::missingRectangle);
    return;
}
    
    for (struct Rectangle& rectangle : *window -> rectangles) {

        rectangle.x = rectangle.baseX * integerScale;
        rectangle.y = rectangle.baseY * integerScale;
        rectangle.width = static_cast<int>(rectangle.baseWidth * integerScale) + 1;
        rectangle.height = static_cast<int>(rectangle.baseHeight * integerScale + 1);

        updateRectangle(rectangle);
    }
}

// undocumented
double getScale(int currentDimension, int baseDimension) {

if (baseDimension <= 0) return 1.0;

    return static_cast<double>(currentDimension) / static_cast<double>(baseDimension);
}

inline int caseWindowCreated(
    HWND windowHandle, 
    WindowProcedureData &windowProcedureData
) {
    windowProcedureData.windowCount++;

    RECT rectangle;
    GetClientRect(windowHandle, &rectangle);
    windowProcedureData.previousWindowWidth = rectangle.right - rectangle.left;
    windowProcedureData.previousWindowHeight = rectangle.bottom - rectangle.top;

    return 0;
}

inline int caseWindowDestroyed(WindowProcedureData &windowProcedureData) {
    windowProcedureData.windowCount--;
    
    if (windowProcedureData.windowCount == 0) PostQuitMessage(0);

    return 0;
}

inline void caseWindowResize(
    HWND windowHandle, 
    LPARAM longParameter, 
    WindowProcedureData &windowProcedureData
) {
    Window* window = getWindow(windowHandle);

    if (!window) {
        logError(ErrorType::missingWindow);
        return;
    };

    int newWidth = LOWORD(longParameter);
    int newHeight = HIWORD(longParameter);

    double scaleX = getScale(newWidth, 800); 
    double scaleY = getScale(newHeight, 600);

    double uniformScale = scaleX; 
    if (scaleY < scaleX) uniformScale = scaleY;

    int integerScale = static_cast<int>(uniformScale);
    if (integerScale < 1) integerScale = 1;
    

    scaleRectangles(integerScale, window);

    updatePreviousWindowDimensions(newWidth, newHeight, windowProcedureData);
    
    InvalidateRect(windowHandle, nullptr, TRUE); 
}

inline int casePaint(HWND windowHandle) {
    Window* window = getWindow(windowHandle);
    PAINTSTRUCT paintStructure;
    HDC handleDeviceContext = BeginPaint(windowHandle, &paintStructure);

    RECT clientArea;
    GetClientRect(windowHandle, &clientArea);
    int width = clientArea.right - clientArea.left;
    int height = clientArea.bottom - clientArea.top;

    // double buffering
    HDC memoryDeviceContext = CreateCompatibleDC(handleDeviceContext);
    HBITMAP memoryBitmap = CreateCompatibleBitmap(handleDeviceContext, width, height);
    SelectObject(memoryDeviceContext, memoryBitmap);

    HBRUSH backgroundBrush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memoryDeviceContext, &clientArea, backgroundBrush);
    DeleteObject(backgroundBrush);

    if (window -> rectangles) {
        for (struct Rectangle rectangle : *window -> rectangles) {
            fillRectangle(memoryDeviceContext, rectangle);
        }
    }

    BitBlt(handleDeviceContext, 0, 0, width, height, memoryDeviceContext, 0, 0, SRCCOPY);

    DeleteObject(memoryBitmap);
    DeleteDC(memoryDeviceContext);
    EndPaint(windowHandle, &paintStructure);

    return 0;
}

void caseKeyDown(WPARAM wordParameter, const Keybind keybinds) {
    for (const std::pair<WPARAM, std::function<void()>> pair : keybinds) {
        if (wordParameter != pair.first) continue;
        pair.second();
    }
}

inline LRESULT CALLBACK windowProcedure(
    HWND windowHandle,
    UINT message,
    WPARAM wordParameter,
    LPARAM longParameter
) {

    if (message == WM_NCCREATE) {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(longParameter);
        SetWindowLongPtrW(windowHandle, GWLP_USERDATA, (LONG_PTR)pCreate -> lpCreateParams);
        return TRUE; 
    }

    Window* window = getWindow(windowHandle);
    
if (!window) return DefWindowProc(windowHandle, message, wordParameter, longParameter);


    switch (message) {
        case WM_CREATE:
            caseWindowCreated(windowHandle, window -> windowProcedureData);
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            casePaint(windowHandle);
            break;

        case WM_SIZE:
            caseWindowResize(windowHandle, longParameter, window -> windowProcedureData);
            break;

        case WM_DESTROY:
            caseWindowDestroyed(window -> windowProcedureData);
            break;

        case WM_KEYDOWN:
            caseKeyDown(wordParameter, window -> keybinds);
            break;

        default:
            return DefWindowProc(windowHandle, message, wordParameter, longParameter);
    }
    return DefWindowProc(windowHandle, message, wordParameter, longParameter);
}

#endif