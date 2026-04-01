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
#include "rectangle.hpp"
#include "structures.hpp"
#include "typedefs.hpp"
#include <windows.h>
#include <vector>
#include <map>
#include <functional>
#include <memory>

LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);

bool register_window_class(HINSTANCE handle_to_instance, LPCWSTR class_name) {
    WNDCLASSW window_class = {};
    window_class.lpfnWndProc = window_procedure;
    window_class.hInstance = handle_to_instance;
    window_class.lpszClassName = class_name;
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    return RegisterClassW(&window_class) != 0;
}

bool create_window(HINSTANCE handle_instance, WINDOW& window) {
    window.handle_to_window = CreateWindowExW(
        window.optional_window_styles,
        window.window_class,
        window.window_title,
        window.window_style,
        window.window_x, window.window_y,
        window.window_width, window.window_height,
        window.parent_window,
        window.window_menu,
        handle_instance,
        static_cast<LPVOID>(&window)
    );

    return window.handle_to_window != nullptr; 
}

int add_rectangle(RECTANGLE* rectangle, WINDOW &window) { 

if (!rectangle) return 1;

    window.rectangles -> push_back(*rectangle);
    
if (!window.handle_to_window) return 1;
    // trigger repaint
    InvalidateRect(window.handle_to_window, nullptr, TRUE); // erases background
    UpdateWindow(window.handle_to_window); // redraw
    
    return 0;
}

int show_window(int show_command, HWND handle_window) {

if (!handle_window) return 1;

    ShowWindow(handle_window, show_command);
    UpdateWindow(handle_window);

    return 0;
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

WINDOW* get_window(HWND handle_to_window) {
    LONG_PTR long_ptr = GetWindowLongPtr(handle_to_window, GWLP_USERDATA); 
    return reinterpret_cast<WINDOW*>(long_ptr);
}

int store_window(HWND handle_to_window, LPARAM long_parameter) {
    CREATESTRUCT* created_window_information = reinterpret_cast<CREATESTRUCT*>(long_parameter); 

    WINDOW* window = reinterpret_cast<WINDOW*>(created_window_information -> lpCreateParams);

    SetWindowLongPtr(handle_to_window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&*window));

    return 0;
}

#include "window_procedure.hpp"

#endif