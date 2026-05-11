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
#include "enum.hpp"

#include <windows.h>
#include <vector>
#include <memory>
#include <chrono>
#include <source_location>

LRESULT CALLBACK window_procedure(HWND, UINT, WPARAM, LPARAM);

struct _ERROR {
    ERROR_TYPE type;
    std::chrono::system_clock::time_point timestamp;
    std::string caller;
    std::string file_path;

    _ERROR(
        ERROR_TYPE p_type,
        std::source_location location
    ): 
    type(p_type),
    timestamp(std::chrono::system_clock::now()),
    caller(location.function_name()),
    file_path(location.file_name()){

    }    
};



struct _BITMAP {
    bool is_textured = false;
    std::vector<COLORREF> pixels;
    int width = 0;
    int height = 0;

    void clear() {
        is_textured = false;
        pixels.clear();
        width = 0;
        height = 0;
    }

    void add(COLORREF pixel) {
        pixels.push_back(pixel);
    }

    bool empty() {
        return (pixels.size() == 0);
    }

    void reallocate(int number) {
        pixels.reserve(number);
    }
};



struct RECTANGLE {
    double x = 0; 
    double y = 0;
    int width = 1; 
    int height = 1;

    double base_x = 0; 
    double base_y = 0;
    int base_width = 1; 
    int base_height = 1;

    RECT raw_rectangle;
    COLORREF color = RGB(255, 255, 255);
    HBRUSH brush = nullptr;

    _BITMAP bitmap;

    RECTANGLE(
        int p_x = 0, 
        int p_y = 0, 
        int p_width = 1,
        int p_height = 1,
        COLORREF p_color = RGB(255, 255, 255)
    ): 
    x(p_x), 
    y(p_y), 
    width(p_width), 
    height(p_height), 
    base_x(p_x), 
    base_y(p_y), 
    base_width(p_width), 
    base_height(p_height), 
    color(p_color) {
        raw_rectangle = {(long)x, (long)y, (long)(x + width), (long)(y + height)};
    }
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
    std::unique_ptr<std::vector<RECTANGLE>> rectangles = {};
    DWORD optional_styles = 0;
    LPCWSTR type = L"class";
    LPCWSTR title = L"window";
    DWORD style = WS_OVERLAPPEDWINDOW;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = 800;
    int height = 600;
    HWND window_handle = nullptr;
    HWND parent_window = nullptr;
    HMENU menu = nullptr;
    LPVOID additional_application_data = nullptr;
    KEYBIND keybinds = {};
    WINDOW_PROCEDURE_DATA window_procedure_data = {};

    WINDOW(
        std::unique_ptr<std::vector<RECTANGLE>> p_rectangles = nullptr,
        DWORD p_optional_styles = 0,
        LPCWSTR p_type = L"class",
        LPCWSTR p_title = L"window",
        DWORD p_style = WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        int p_x = CW_USEDEFAULT,
        int p_y = CW_USEDEFAULT,
        int p_width = 800,
        int p_height = 600,
        HWND p_window_handle = nullptr,
        HWND p_parent_window = nullptr,
        HMENU p_menu = nullptr,
        LPVOID p_additional_application_data = nullptr,
        KEYBIND p_keybinds = {},
        WINDOW_PROCEDURE_DATA p_window_procedure_data = {}
    ) : 
        rectangles(std::move(p_rectangles)),
        optional_styles(p_optional_styles),
        type(p_type),
        title(p_title),
        style(p_style),
        x(p_x),
        y(p_y),
        width(std::max(0, p_width)),
        height(std::max(0, p_height)),
        window_handle(p_window_handle),
        parent_window(p_parent_window),
        menu(p_menu),
        additional_application_data(p_additional_application_data),
        keybinds(p_keybinds),
        window_procedure_data(p_window_procedure_data)
    {
        if (!rectangles) {
            rectangles = std::make_unique<std::vector<RECTANGLE>>();
        }

        HINSTANCE instance = GetModuleHandleW(NULL);

        WNDCLASSW window_class = {};
        window_class.lpfnWndProc = window_procedure;
        window_class.hInstance = instance;
        window_class.lpszClassName = type;
        window_class.hCursor = LoadCursorW(NULL, (LPCWSTR)IDC_ARROW);

        RegisterClassW(&window_class);

        window_handle = CreateWindowExW(
            optional_styles, type, title,
            style,
            x, y, width, height,
            parent_window, menu, instance, this
        );
    }
};

#endif