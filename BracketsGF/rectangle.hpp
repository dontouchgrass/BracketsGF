#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef RECTANGLE_HPP
#define RECTANGLE_HPP


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "structures.hpp"
#include "errors.hpp"

#include <windows.h>
#include <vector>
#include <memory>

inline void update_brush(RECTANGLE& rectangle) {

    if (rectangle.brush) DeleteObject(rectangle.brush);

    rectangle.brush = CreateSolidBrush(rectangle.color);
}


void update_rectangle(RECTANGLE &rectangle) {
    rectangle.raw_rectangle.left = static_cast<long>(rectangle.x);
    rectangle.raw_rectangle.top = static_cast<long>(rectangle.y);
    rectangle.raw_rectangle.right = static_cast<long>(rectangle.x + rectangle.width);
    rectangle.raw_rectangle.bottom = static_cast<long>(rectangle.y + rectangle.height);

    update_brush(rectangle);
}

void fill_rectangle(HDC handle_device_context, RECTANGLE& rectangle) {
    if ((!rectangle.bitmap.is_textured || rectangle.bitmap.empty())) {
        
        if (!rectangle.brush) rectangle.brush = CreateSolidBrush(rectangle.color);
    
        FillRect(handle_device_context, &rectangle.raw_rectangle, rectangle.brush);

        log_error(ERROR_TYPE::INVALID_BITMAP);

        return;
    }

    // High-speed pixel transfer
    BITMAPINFO bitmap_information = {0};
    bitmap_information.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_information.bmiHeader.biWidth = rectangle.bitmap.width;
    bitmap_information.bmiHeader.biHeight = -rectangle.bitmap.height; // Negative for top-down
    bitmap_information.bmiHeader.biPlanes = 1;
    bitmap_information.bmiHeader.biBitCount = 32;
    bitmap_information.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(
        handle_device_context,
        rectangle.raw_rectangle.left,
        rectangle.raw_rectangle.top,
        rectangle.raw_rectangle.right - rectangle.raw_rectangle.left,
        rectangle.raw_rectangle.bottom - rectangle.raw_rectangle.top,
        0, 0, rectangle.bitmap.width, rectangle.bitmap.height,
        rectangle.bitmap.pixels.data(),
        &bitmap_information,
        DIB_RGB_COLORS,
        SRCCOPY
    );
    
}

void draw_rectangles(HDC handle_device_context, std::vector<RECTANGLE>* rectangles) {
    if (!rectangles) {
        log_error(ERROR_TYPE::MISSING_RECTANGLE);
        return;
    }

    for (RECTANGLE rectangle : *rectangles) {
        fill_rectangle(handle_device_context, rectangle);
    }
}

#endif