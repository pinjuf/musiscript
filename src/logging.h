#pragma once

#include <cstdint>
#include <string>

#include "config.h"

#define RESET_COLOR "\x1B[0m"

enum LOGLEVEL {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

constexpr const char ** LOGPREFIXES = (const char *[]) {
    (char *)"[DEBUG]  ",
    (char *)"[INFO]   ",
    (char *)"[WARNING]",
    (char *)"[ERROR]  ",
    (char *)"[FATAL]  ",
};

constexpr const char ** LOGCOLORS = (const char *[]) {
#if DO_COLOR_LOG
    (char *)"\x1b[36m", // Blue
    (char *)"\x1b[32m", // Green
    (char *)"\x1b[33m", // Yellow
    (char *)"\x1b[31m", // Red
    (char *)"\x1b[35m", // Magenta
#else
    (char *)"",
    (char *)"",
    (char *)"",
    (char *)"",
    (char *)"",
#endif
};

LOGLEVEL getloglevel();
void setloglevel(LOGLEVEL level);
void log(LOGLEVEL level, const char * msg, bool incl_line_num = false);
void log(LOGLEVEL level, std::string msg, bool incl_line_num = false);
void set_line_num_ptr(std::size_t * line_num_ptr);
