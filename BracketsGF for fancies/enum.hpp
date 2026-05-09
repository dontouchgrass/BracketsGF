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

enum class ErrorType {
    missingRectangle,
    missingWindowHandle,
    missingWindow,
    missingLog,
    invalidDimension,
    invalidBitmap,
    testType
};

#endif