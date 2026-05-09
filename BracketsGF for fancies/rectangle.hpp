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

inline void updateBrush(struct Rectangle& rectangle) {

    if (rectangle.brush) DeleteObject(rectangle.brush);

    rectangle.brush = CreateSolidBrush(rectangle.color);
}


void updateRectangle(struct Rectangle &rectangle) {
    rectangle.rawRectangle.left = static_cast<long>(rectangle.x);
    rectangle.rawRectangle.top = static_cast<long>(rectangle.y);
    rectangle.rawRectangle.right = static_cast<long>(rectangle.x + rectangle.width);
    rectangle.rawRectangle.bottom = static_cast<long>(rectangle.y + rectangle.height);

    updateBrush(rectangle);
}

void fillRectangle(HDC handleDeviceContext, struct Rectangle& rectangle) {
    if ((!rectangle.bitmap.isTextured || rectangle.bitmap.empty())) {
        
        if (!rectangle.brush) rectangle.brush = CreateSolidBrush(rectangle.color);
    
        FillRect(handleDeviceContext, &rectangle.rawRectangle, rectangle.brush);

        logError(ErrorType::invalidBitmap);

        return;
    }

    BITMAPINFO bitmapInformation = {0};
    bitmapInformation.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInformation.bmiHeader.biWidth = rectangle.bitmap.width;
    bitmapInformation.bmiHeader.biHeight = -rectangle.bitmap.height; // negative for top-down
    bitmapInformation.bmiHeader.biPlanes = 1;
    bitmapInformation.bmiHeader.biBitCount = 32;
    bitmapInformation.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(
        handleDeviceContext,
        rectangle.rawRectangle.left,
        rectangle.rawRectangle.top,
        rectangle.rawRectangle.right - rectangle.rawRectangle.left,
        rectangle.rawRectangle.bottom - rectangle.rawRectangle.top,
        0, 0, rectangle.bitmap.width, rectangle.bitmap.height,
        rectangle.bitmap.pixels.data(),
        &bitmapInformation,
        DIB_RGB_COLORS,
        SRCCOPY
    );
    
}

void drawRectangles(HDC handleDeviceContext, std::vector<struct Rectangle>* rectangles) {
    if (!rectangles) {
        logError(ErrorType::missingRectangle);
        return;
    }

    for (struct Rectangle rectangle : *rectangles) {
        fillRectangle(handleDeviceContext, rectangle);
    }
}

#endif