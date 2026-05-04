#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#ifndef ERRORS_HPP
#define ERRORS_HPP


#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <fstream>
#include <string>
#include <vector>
#include <format>

#include "settings.hpp"
#include "enum.hpp"
#include "structures.hpp"

std::string convert(ERROR_TYPE type) {
    switch (type) {
        case ERROR_TYPE::MISSING_RECTANGLE:
            return "MISSING_RECTANGLE";
            break;
        case ERROR_TYPE::MISSING_WINDOW_HANDLE:
            return "MISSING_WINDOW_HANDLE";
            break;
        case ERROR_TYPE::MISSING_WINDOW:
            return "MISSING_WINDOW";
            break;
        case ERROR_TYPE::MISSING_LOG:
            return "MISSING_LOG";
            break;
        case ERROR_TYPE::INVALID_DIMENSION:
            return "INVALID_DIMENSION";
            break;
        case ERROR_TYPE::INVALID_BITMAP:
            return "INVALID_BITMAP";
            break;
        case ERROR_TYPE::TEST_TYPE:
            return "TEST_TYPE";
            break;
        default:
            return "UNKNOWN_TYPE_ \"" + std::to_string(static_cast<int>(type)) + "\"";
    }
}

static std::vector<_ERROR> failsafe_log = {};

std::string format_error(_ERROR error) {
    // We use a raw string literal to satisfy the compiler's requirement for a constant
    // We pass the separator as an argument so we don't have to use the '+' operator
    return std::format("{} {} {} {} {} {} {:%Y-%m-%d %H:%M:%S}", 
        convert(error.type), 
        FORMAT_ERROR_SEPARATOR, 
        error.caller, 
        FORMAT_ERROR_SEPARATOR, 
        error.file_path, 
        FORMAT_ERROR_SEPARATOR, 
        error.timestamp
    );
}

void log_error(ERROR_TYPE name, std::source_location location = std::source_location::current()) {
    _ERROR error = {name, location};
    std::ofstream log(std::string(LOG_PATH), std::ios::app);

if (!log.is_open()) {
    _ERROR missing_log = {ERROR_TYPE::MISSING_LOG, location};
    failsafe_log.push_back(missing_log); 
    failsafe_log.push_back(error); 
    return;
}

    log << format_error(error) << std::endl;
    log.close();
}

#endif