#include "logging.h"

LOGLEVEL loglevel = LOG_INFO;

LOGLEVEL getloglevel() {
    return loglevel;
}

void setloglevel(LOGLEVEL level) {
    loglevel = level;
}

void log(LOGLEVEL level, const char * msg) {
    if (level >= loglevel) {
        std::cout << logprefixes[level] << " " << msg << std::endl;
    }
}
