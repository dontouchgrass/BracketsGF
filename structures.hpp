#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef STRUCTURES_HPP
#define STRUCTURES_HPP


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "typedefs.hpp"

#include <windows.h>
#include <vector>
#include <memory>

struct RECTANGLE {
    double x = 0;
    double y = 0;
    int width = 1;
    int height = 1;

    double base_x = 0;
    double base_y = 0;
    int base_width = 1;
    int base_height = 1;

    RECT raw_rectangle = {
        static_cast<long>(x),
        static_cast<long>(y),
        static_cast<long>(x + width),
        static_cast<long>(y + height)
    };
    COLORREF color = RGB(255, 255, 255);
    HBRUSH brush = nullptr;
};

struct WINDOW_PROCEDURE_DATA {
    int window_count;
    KEYBIND keybinds;

    int base_window_width;
    int base_window_height;

    int previous_window_width;
    int previous_window_height;
};

struct WINDOW {
    std::unique_ptr<std::vector<RECTANGLE>> rectangles;
    DWORD optional_window_styles = 0;
    LPCWSTR window_class = L"window class";
    LPCWSTR window_title = L"window 1";
    DWORD window_style = WS_OVERLAPPEDWINDOW;
    int window_x = CW_USEDEFAULT;
    int window_y = CW_USEDEFAULT;
    int window_width = 800;
    int window_height = 600;
    HWND handle_to_window = nullptr;
    HWND parent_window = nullptr;
    HMENU window_menu = nullptr;
    LPVOID additional_application_data = nullptr;
    KEYBIND keybinds;
    WINDOW_PROCEDURE_DATA window_procedure_data;
};

#endif