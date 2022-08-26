#ifndef VOICES_H
#define VOICES_H

#include <vector>
#include <map>
#include <stack>
#include <string>

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

        size_t counter = 0;

        std::vector<StereoSample> samples;

    private:
        std::vector<Effect> effects;
        std::map<std::string,std::string> defs;

        std::stack<std::streampos> substack;
        std::map<std::string,std::streampos> subs;

        std::stack<std::streampos> repstack;
        std::stack<int> reps;
        std::stack<bool> ifs;

        std::string replace_defs_with_vals(std::string line);
        std::string replace_infix_codepts_with_vals(std::string line);
        std::string remove_comments(std::string line);
};

#endif
