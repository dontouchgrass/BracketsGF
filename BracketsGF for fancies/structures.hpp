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



struct Error {
    ErrorType type;
    std::chrono::system_clock::time_point timestamp;
    std::string caller;
    std::string filePath;

    Error(
        ErrorType pType,
        std::source_location location
    ): 
    type(pType),
    timestamp(std::chrono::system_clock::now()),
    caller(location.function_name()),
    filePath(location.file_name()){

    }    
};



struct Bitmap {
    bool isTextured = false;
    std::vector<COLORREF> pixels;
    int width = 0;
    int height = 0;

    void clear() {
        isTextured = false;
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



struct Rectangle {
    double x = 0; 
    double y = 0;
    int width = 1; 
    int height = 1;

    double baseX = 0; 
    double baseY = 0;
    int baseWidth = 1; 
    int baseHeight = 1;

    RECT rawRectangle;
    COLORREF color = RGB(255, 255, 255);
    HBRUSH brush = nullptr;

    Bitmap bitmap;

    Rectangle(
        int pX = 0, 
        int pY = 0, 
        int pWidth = 1,
        int pHeight = 1,
        COLORREF pColor = RGB(255, 255, 255)
    ): 
    x(pX), 
    y(pY), 
    width(pWidth), 
    height(pHeight), 
    baseX(pX), 
    baseY(pY), 
    baseWidth(pWidth), 
    baseHeight(pHeight), 
    color(pColor) {
        rawRectangle = {(long)x, (long)y, (long)(x + width), (long)(y + height)};
    }
};



struct WindowProcedureData {
    int windowCount;
    Keybind keybinds;

    int baseWindowWidth;
    int baseWindowHeight;

    int previousWindowWidth;
    int previousWindowHeight;
};



struct Window {
    std::unique_ptr<std::vector<struct Rectangle>> rectangles = {};
    DWORD optionalStyles = 0;
    LPCWSTR type = L"Cass";
    LPCWSTR title = L"Window";
    DWORD style = WS_OVERLAPPEDWINDOW;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;
    int width = 800;
    int height = 600;
    HWND windowHandle = nullptr;
    HWND parentWindow = nullptr;
    HMENU menu = nullptr;
    LPVOID additionalApplicationData = nullptr;
    Keybind keybinds = {};
    WindowProcedureData windowProcedureData = {};

    Window(
        std::unique_ptr<std::vector<struct Rectangle>> pRectangles = nullptr,
        DWORD pOptionalStyles = 0,
        LPCWSTR pType = L"Class",
        LPCWSTR pTitle = L"Window",
        DWORD pStyle = WS_OVERLAPPEDWINDOW,
        int pX = CW_USEDEFAULT,
        int pY = CW_USEDEFAULT,
        int pWidth = 800,
        int pHeight = 600,
        HWND pWindowHandle = nullptr,
        HWND pParentWindow = nullptr,
        HMENU pMenu = nullptr,
        LPVOID pAdditionalApplicationData = nullptr,
        Keybind pKeybinds = {},
        WindowProcedureData pWindowProcedureData = {}
    ) : 
        rectangles(std::move(pRectangles)),
        optionalStyles(pOptionalStyles),
        type(pType),
        title(pTitle),
        style(pStyle),
        x(pX),
        y(pY),
        width(std::max(0, pWidth)),
        height(std::max(0, pHeight)),
        windowHandle(pWindowHandle),
        parentWindow(pParentWindow),
        menu(pMenu),
        additionalApplicationData(pAdditionalApplicationData),
        keybinds(pKeybinds),
        windowProcedureData(pWindowProcedureData)
    {

    }
};

#endif