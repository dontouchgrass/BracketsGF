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

#include "rectangle.hpp"

#include "windows.hpp"
#include <windows.h>
#include <functional>
#include <map>

// undocumented
int update_previous_window_dimensions(int new_width, int new_height, WINDOW_PROCEDURE_DATA& window_procedure_data) {

if (new_width <= 0 || new_height <= 0) return 1;

    window_procedure_data.previous_window_width = new_width;
    window_procedure_data.previous_window_height = new_height;

    return 0;
}

// undocumented
int scale_rectangles(double integer_scale, WINDOW* window) {

if (!window || !window->rectangles) return 1;
    
    for (RECTANGLE& rectangle : *window -> rectangles) {

        rectangle.x = rectangle.base_x * integer_scale;
        rectangle.y = rectangle.base_y * integer_scale;
        rectangle.width = static_cast<int>(rectangle.base_width * integer_scale) + 1;
        rectangle.height = static_cast<int>(rectangle.base_height * integer_scale + 1);

        update_rectangle(rectangle);
    }

    return 0;
}

// undocumented
double get_scale(int current_dimension, int base_dimension) {

if (base_dimension <= 0) return 1.0;

    return static_cast<double>(current_dimension) / static_cast<double>(base_dimension);
}

inline int case_window_created(
    HWND handle_to_window, 
    LPARAM long_parameter, 
    WINDOW_PROCEDURE_DATA &window_procedure_data
) {
    window_procedure_data.window_count++;

    RECT rectangle;
    GetClientRect(handle_to_window, &rectangle);
    window_procedure_data.previous_window_width = rectangle.right - rectangle.left;
    window_procedure_data.previous_window_height = rectangle.bottom - rectangle.top;

    return 0;
}

inline int case_window_destroyed(WINDOW_PROCEDURE_DATA &window_procedure_data) {
    window_procedure_data.window_count--;
    
    if (window_procedure_data.window_count == 0) PostQuitMessage(0);

    return 0;
}

inline int case_window_resize(
    HWND handle_to_window, 
    LPARAM long_parameter, 
    WINDOW_PROCEDURE_DATA &window_procedure_data
) {
    WINDOW* window = get_window(handle_to_window);

    if (!window) return 1;

    int new_width = LOWORD(long_parameter);
    int new_height = HIWORD(long_parameter);

    // DEBUG:
    wchar_t buf[256];
    swprintf(buf, 256, L"New: %d | Prev: %d\n", new_width, window_procedure_data.previous_window_width);
    OutputDebugStringW(buf);

    double scale_x = get_scale(new_width, 800); 
    double scale_y = get_scale(new_height, 600);

    double uniform_scale = scale_x; 
    if (scale_y < scale_x) uniform_scale = scale_y;

    int integer_scale = static_cast<int>(uniform_scale);
    if (integer_scale < 1) integer_scale = 1;
    

    scale_rectangles(integer_scale, window);

    update_previous_window_dimensions(new_width, new_height, window_procedure_data);
    
    InvalidateRect(handle_to_window, nullptr, TRUE); 

    return 0;
}

inline int case_paint(HWND handle_to_window) {
    std::vector<RECTANGLE>* rectangles = &*get_window(handle_to_window) -> rectangles;

    PAINTSTRUCT paint_structure;
    HDC handle_device_context = BeginPaint(handle_to_window, &paint_structure);

    RECT client;
    GetClientRect(handle_to_window, &client);

    HBRUSH background = CreateSolidBrush(RGB(0, 0, 0));
    FillRect(handle_device_context, &client, background);
    DeleteObject(background);

    draw_rectangles(handle_device_context, rectangles);

    EndPaint(handle_to_window, &paint_structure);

    return 0;
}

int case_key_down(WPARAM word_parameter, const KEYBIND keybinds) {
    for (const std::pair<WPARAM, std::function<void()>> pair : keybinds) {
        if (word_parameter != pair.first) continue;
        pair.second();
    }
    return 0;
}

inline LRESULT CALLBACK window_procedure(
    HWND handle_to_window,
    UINT message,
    WPARAM word_parameter,
    LPARAM long_parameter
) {

    if (message == WM_NCCREATE) {
        CREATESTRUCTW* pCreate = reinterpret_cast<CREATESTRUCTW*>(long_parameter);
        SetWindowLongPtrW(handle_to_window, GWLP_USERDATA, (LONG_PTR)pCreate->lpCreateParams);
        return TRUE; 
    }

    WINDOW* window = get_window(handle_to_window);
    
if (!window) return DefWindowProc(handle_to_window, message, word_parameter, long_parameter);

    switch (message) {
        case WM_CREATE:
            case_window_created(handle_to_window, long_parameter, window->window_procedure_data);
            break;

        case WM_ERASEBKGND:
            return 1;

        case WM_PAINT:
            case_paint(handle_to_window);
            break;

        case WM_SIZE:
            case_window_resize(handle_to_window, long_parameter, window -> window_procedure_data);
            break;

        case WM_DESTROY:
            case_window_destroyed(window -> window_procedure_data);
            break;

        case WM_KEYDOWN:
            case_key_down(word_parameter, window -> keybinds);
            break;

        default:
            return DefWindowProc(handle_to_window, message, word_parameter, long_parameter);
    }
    return 0;
}

#endif