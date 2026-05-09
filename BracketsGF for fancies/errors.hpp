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

std::string convert(ErrorType type) {
    switch (type) {
        case ErrorType::missingRectangle:
            return "missingRectangle";
            break;
        case ErrorType::missingWindowHandle:
            return "missingWindowHandle";
            break;
        case ErrorType::missingWindow:
            return "missingWindow";
            break;
        case ErrorType::missingLog:
            return "missingLog";
            break;
        case ErrorType::invalidDimension:
            return "invalidDimension";
            break;
        case ErrorType::invalidBitmap:
            return "invalidBitmap";
            break;
        case ErrorType::testType:
            return "testType";
            break;
        default:
            return "unknownType \"" + std::to_string(static_cast<int>(type)) + "\"";
    }
}

static std::vector<Error> failsafeLog = {};

std::string formatError(Error error) {
    return std::format("{} {} {} {} {} {} {:%Y-%m-%d %H:%M:%S}", 
        convert(error.type), 
        formatErrorSeparator, 
        error.caller, 
        formatErrorSeparator, 
        error.filePath, 
        formatErrorSeparator, 
        error.timestamp
    );
}

void logError(ErrorType name, std::source_location location = std::source_location::current()) {
    Error error = {name, location};
    std::ofstream log(std::string(logPath), std::ios::app);

if (!log.is_open()) {
    Error missingLog = {ErrorType::missingLog, location};
    failsafeLog.push_back(missingLog); 
    failsafeLog.push_back(error); 
    return;
}

    log << formatError(error) << std::endl;
    log.close();
}

#endif