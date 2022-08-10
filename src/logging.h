#ifndef LOGGING_H
#define LOGGING_H

enum LOGLEVEL {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
};

constexpr char ** logprefixes = (char *[]) {
    (char *)"[DEBUG]  ",
    (char *)"[INFO]   ",
    (char *)"[WARNING]",
    (char *)"[ERROR]  ",
    (char *)"[FATAL]  ",
};

LOGLEVEL getloglevel();
void setloglevel(LOGLEVEL level);
void log(LOGLEVEL level, const char * msg);

#endif
