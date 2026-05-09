// build constructors and destructors

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef WINDOWS_HPP
#define WINDOWS_HPP


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#pragma once

#include "rectangle.hpp"
#include "structures.hpp"
#include "typedefs.hpp"

#include <windows.h>
#include <vector>
#include <map>
#include <functional>
#include <memory>

LRESULT CALLBACK windowProcedure(HWND, UINT, WPARAM, LPARAM);

bool registerWindowClass(HINSTANCE instanceHandle, LPCWSTR className) {
    WNDCLASSW windowClass = {};
    windowClass.lpfnWndProc = windowProcedure;
    windowClass.hInstance = instanceHandle;
    windowClass.lpszClassName = className;
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClassW(&windowClass) != 0;
}

bool create_window(HINSTANCE handle_instance, Window& window) {
    window.windowHandle = CreateWindowExW(
        window.optionalStyles,
        window.type,
        window.title,
        window.style,
        window.x, window.y,
        window.width, window.height,
        window.parentWindow,
        window.menu,
        handle_instance,
        static_cast<LPVOID>(&window)
    );

    return window.windowHandle != nullptr; 
}

void addRectangle(struct Rectangle* rectangle, Window &window) { 
if (!rectangle) {
    logError(ErrorType::missingRectangle);
    return;
}

    window.rectangles -> push_back(*rectangle);
    
if (!window.windowHandle) {
    logError(ErrorType::missingWindowHandle);
    return;
}

    // trigger repaint
    InvalidateRect(window.windowHandle, nullptr, TRUE); // erases background
    UpdateWindow(window.windowHandle); // redraw
}

void showWindow(int showCommand, HWND windowHandle) {
if (!windowHandle) {
    logError(ErrorType::missingWindowHandle);
    return;
}

    ShowWindow(windowHandle, showCommand);
    UpdateWindow(windowHandle);
}

int runMessageLoop() {
    MSG message = {};
    while (GetMessage(
        &message,
        nullptr,
        0, // The integer value of the lowest message value to be retrieved.
        0 // The integer value of the highest message value to be retrieved.
        )) {
        TranslateMessage(&message);
        DispatchMessage(&message);
        }
    return static_cast<int>(message.wParam);
}

Window* getWindow(HWND windowHandle) {
    LONG_PTR longPointer = GetWindowLongPtr(windowHandle, GWLP_USERDATA); 
    return reinterpret_cast<Window*>(longPointer);
}

void storeWindow(HWND windowHandle, LPARAM longParameter) {
    CREATESTRUCT* createdWindowInformation = reinterpret_cast<CREATESTRUCT*>(longParameter); 

    Window* window = reinterpret_cast<Window*>(createdWindowInformation -> lpCreateParams);

    SetWindowLongPtr(windowHandle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&*window));
}

#include "window_procedure.hpp"

#endif