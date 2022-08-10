#ifndef LOGGING_H
#define LOGGING_H

#include <cstdint>

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
    (char *)"\x1b[36m",
    (char *)"\x1b[32m",
    (char *)"\x1b[33m",
    (char *)"\x1b[31m",
    (char *)"\x1b[35m",
};

LOGLEVEL getloglevel();
void setloglevel(LOGLEVEL level);
void log(LOGLEVEL level, const char * msg, bool incl_line_num = false);
void set_line_num_ptr(std::size_t * line_num_ptr);

#endif
