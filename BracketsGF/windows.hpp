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

LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);

bool register_window_class(HINSTANCE instance_handle, LPCWSTR class_name) {
    WNDCLASSW window_class = {};
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = instance_handle;
    window_class.lpszClassName = class_name;
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClassW(&window_class) != 0;
}

bool create_window(HINSTANCE handle_instance, WINDOW& window) {
    window.window_handle = CreateWindowExW(
        window.optional_styles,
        window.type,
        window.title,
        window.style,
        window.x, window.y,
        window.width, window.height,
        window.parent_window,
        window.menu,
        handle_instance,
        static_cast<LPVOID>(&window)
    );

    return window.window_handle != nullptr; 
}

void add_rectangle(RECTANGLE* rectangle, WINDOW &window) { 
if (!rectangle) {
    log_error(ERROR_TYPE::MISSING_RECTANGLE);
    return;
}

    window.rectangles -> push_back(*rectangle);
    
if (!window.window_handle) {
    log_error(ERROR_TYPE::MISSING_WINDOW_HANDLE);
    return;
}

    // trigger repaint
    InvalidateRect(window.window_handle, nullptr, TRUE); // erases background
    UpdateWindow(window.window_handle); // redraw
}

void show_window(int show_command, HWND window_handle) {
if (!window_handle) {
    log_error(ERROR_TYPE::MISSING_WINDOW_HANDLE);
    return;
}

    ShowWindow(window_handle, show_command);
    UpdateWindow(window_handle);
}

int run_message_loop() {
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

WINDOW* get_window(HWND window_handle) {
    LONG_PTR long_ptr = GetWindowLongPtr(window_handle, GWLP_USERDATA); 
    return reinterpret_cast<WINDOW*>(long_ptr);
}

int store_window(HWND window_handle, LPARAM long_parameter) {
    CREATESTRUCT* created_window_information = reinterpret_cast<CREATESTRUCT*>(long_parameter); 

    WINDOW* window = reinterpret_cast<WINDOW*>(created_window_information -> lpCreateParams);

    SetWindowLongPtr(window_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&*window));

    return 0;
}

#include "window_procedure.hpp"

#endif