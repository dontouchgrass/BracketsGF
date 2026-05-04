#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef TYPEDEFS_HPP
#define TYPEDEFS_HPP

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <map>
#include <functional>

typedef std::map<WPARAM, std::function<void()>> KEYBIND;

#endif