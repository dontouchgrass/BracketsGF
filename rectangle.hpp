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

#include <windows.h>
#include <vector>
#include <memory>

int fill_rectangle(HDC handle_to_device_context, const RECTANGLE& rectangle) {

if (!rectangle.brush) return 1;

    FillRect(handle_to_device_context, &rectangle.raw_rectangle, rectangle.brush);

    return 0;
}

inline int update_brush(RECTANGLE& rectangle) {

    if (rectangle.brush) DeleteObject(rectangle.brush);

    rectangle.brush = CreateSolidBrush(rectangle.color);

    return 0;
}


int update_rectangle(RECTANGLE &rectangle) {
    rectangle.raw_rectangle.left = static_cast<long>(rectangle.x);
    rectangle.raw_rectangle.top = static_cast<long>(rectangle.y);
    rectangle.raw_rectangle.right = static_cast<long>(rectangle.x + rectangle.width);
    rectangle.raw_rectangle.bottom = static_cast<long>(rectangle.y + rectangle.height);

    update_brush(rectangle);
    return 0;
}

int draw_rectangles(HDC handle_device_context, std::vector<RECTANGLE>* rectangles) {
    
if (!rectangles) return 1;

    for (const RECTANGLE& rectangle : *rectangles) {
        if (rectangle.raw_rectangle.right <= rectangle.raw_rectangle.left || 
            rectangle.raw_rectangle.bottom <= rectangle.raw_rectangle.top) {
            continue;
        }
        
        fill_rectangle(handle_device_context, rectangle);
    }
    return 0;
}

#endif