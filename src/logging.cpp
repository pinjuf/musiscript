#include <iostream>

#include "config.h"
#include "logging.h"

LOGLEVEL loglevel = DEF_LOG_LEVEL;
size_t * line_num;

LOGLEVEL getloglevel() {
    return loglevel;
}

void setloglevel(LOGLEVEL level) {
    loglevel = level;
}

void log(LOGLEVEL level, const char * msg, bool incl_line_num) {
    if (level >= loglevel) {
        if (incl_line_num)
            std::cout << LOGCOLORS[level] << LOGPREFIXES[level] << RESET_COLOR << " L" << *line_num << ": " << msg << std::endl;
        else
            std::cout << LOGCOLORS[level] << LOGPREFIXES[level] << RESET_COLOR << " " << msg << std::endl;
    }
}

void log(LOGLEVEL level, std::string msg, bool incl_line_num) {
    log(level, msg.c_str(), incl_line_num);
}

void set_line_num_ptr(size_t * line_num_ptr) {
    line_num = line_num_ptr;
}
