#ifndef VOICES_H
#define VOICES_H

#include <vector>
#include <string.h>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <map>
#include <regex>

#include "wav.h"
#include "sounds.h"
#include "notes.h"
#include "effects.h"
#include "notes.h"

class Voice {
    public:
        void voice_from_file(char * path);

        double pan    = 0.5;
        double volume = 1;
        double speed  = 1;
        int transpose = 0;
        int sound     = SILENCE;

        void read_from_file(char * filename, std::vector<StereoSample> * outsamples);

        std::vector<Effect> effects;
        std::map<std::string,std::string> defs;
};

#endif
