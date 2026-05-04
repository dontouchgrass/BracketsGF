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
void update_previous_window_dimensions(
    int new_width, 
    int new_height, 
    WINDOW_PROCEDURE_DATA& window_procedure_data
) {

if (new_width <= 0 || new_height <= 0) {
    log_error(ERROR_TYPE::INVALID_DIMENSION);
    return;
}

    window_procedure_data.previous_window_width = new_width;
    window_procedure_data.previous_window_height = new_height;
}

// undocumented
void scale_rectangles(double integer_scale, WINDOW* window) {

if (!window) {
    log_error(ERROR_TYPE::MISSING_WINDOW);
    return;
}

if (!window -> rectangles) {
    log_error(ERROR_TYPE::MISSING_RECTANGLE);
    return;
}
    
    for (RECTANGLE& rectangle : *window -> rectangles) {

        rectangle.x = rectangle.base_x * integer_scale;
        rectangle.y = rectangle.base_y * integer_scale;
        rectangle.width = static_cast<int>(rectangle.base_width * integer_scale) + 1;
        rectangle.height = static_cast<int>(rectangle.base_height * integer_scale + 1);

        update_rectangle(rectangle);
    }
}

// undocumented
double get_scale(int current_dimension, int base_dimension) {

if (base_dimension <= 0) return 1.0;

    return static_cast<double>(current_dimension) / static_cast<double>(base_dimension);
}

inline int case_window_created(
    HWND window_handle, 
    LPARAM long_parameter, 
    WINDOW_PROCEDURE_DATA &window_procedure_data
) {
    window_procedure_data.window_count++;

    RECT rectangle;
    GetClientRect(window_handle, &rectangle);
    window_procedure_data.previous_window_width = rectangle.right - rectangle.left;
    window_procedure_data.previous_window_height = rectangle.bottom - rectangle.top;

    return 0;
}

inline int case_window_destroyed(WINDOW_PROCEDURE_DATA &window_procedure_data) {
    window_procedure_data.window_count--;
    
    if (window_procedure_data.window_count == 0) PostQuitMessage(0);

    return 0;
}

inline void case_window_resize(
    HWND window_handle, 
    LPARAM long_parameter, 
    WINDOW_PROCEDURE_DATA &window_procedure_data
) {
    WINDOW* window = get_window(window_handle);

    if (!window) {
        log_error(ERROR_TYPE::MISSING_WINDOW);
        return;
    };

    int new_width = LOWORD(long_parameter);
    int new_height = HIWORD(long_parameter);

    double scale_x = get_scale(new_width, 800); 
    double scale_y = get_scale(new_height, 600);

    double uniform_scale = scale_x; 
    if (scale_y < scale_x) uniform_scale = scale_y;

    int integer_scale = static_cast<int>(uniform_scale);
    if (integer_scale < 1) integer_scale = 1;
    

    scale_rectangles(integer_scale, window);

    update_previous_window_dimensions(new_width, new_height, window_procedure_data);
    
    InvalidateRect(window_handle, nullptr, TRUE); 
}

inline int case_paint(HWND window_handle) {
    WINDOW* window = get_window(window_handle);
    PAINTSTRUCT paint_structure;
    HDC handle_device_context = BeginPaint(window_handle, &paint_structure);

    RECT client_area;
    GetClientRect(window_handle, &client_area);
    int width = client_area.right - client_area.left;
    int height = client_area.bottom - client_area.top;

    // double buffering
    HDC memory_device_context = CreateCompatibleDC(handle_device_context);
    HBITMAP memory_bitmap = CreateCompatibleBitmap(handle_device_context, width, height);
    SelectObject(memory_device_context, memory_bitmap);

    HBRUSH background_brush = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(memory_device_context, &client_area, background_brush);
    DeleteObject(background_brush);

    if (window -> rectangles) {
        for (RECTANGLE rectangle : *window -> rectangles) {
            fill_rectangle(memory_device_context, rectangle);
        }
    }

    BitBlt(handle_device_context, 0, 0, width, height, memory_device_context, 0, 0, SRCCOPY);

    DeleteObject(memory_bitmap);
    DeleteDC(memory_device_context);
    EndPaint(window_handle, &paint_structure);

    return 0;
}

void case_key_down(WPARAM word_parameter, const KEYBIND keybinds) {
    for (const std::pair<WPARAM, std::function<void()>> pair : keybinds) {
        if (word_parameter != pair.first) continue;
        pair.second();
    }
}

inline LRESULT CALLBACK window_procedure(
    HWND window_handle,
    UINT message,
    WPARAM word_parameter,
    LPARAM long_parameter
) {

    if (message == WM_NCCREATE) {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(long_parameter);
        SetWindowLongPtrW(window_handle, GWLP_USERDATA, (LONG_PTR)pCreate -> lpCreateParams);
        return TRUE; 
    }

    WINDOW* window = get_window(window_handle);
    
if (!window) return DefWindowProc(window_handle, message, word_parameter, long_parameter);


    switch (message) {
        case WM_CREATE:
            case_window_created(window_handle, long_parameter, window -> window_procedure_data);
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            case_paint(window_handle);
            break;

        case WM_SIZE:
            case_window_resize(window_handle, long_parameter, window -> window_procedure_data);
            break;

        case WM_DESTROY:
            case_window_destroyed(window -> window_procedure_data);
            break;

        case WM_KEYDOWN:
            case_key_down(word_parameter, window -> keybinds);
            break;

        default:
            return DefWindowProc(window_handle, message, word_parameter, long_parameter);
    }
    return DefWindowProc(window_handle, message, word_parameter, long_parameter);
}

#endif