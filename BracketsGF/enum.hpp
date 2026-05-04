#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef ENUM_HPP
#define ENUM_HPP


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

enum class ERROR_TYPE {
    MISSING_RECTANGLE,
    MISSING_WINDOW_HANDLE,
    MISSING_WINDOW,
    MISSING_LOG,
    INVALID_DIMENSION,
    INVALID_BITMAP,
    TEST_TYPE
};

#endif