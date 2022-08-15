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

    private:
        std::vector<Effect> effects;
        std::map<std::string,std::string> defs;

        std::stack<std::streampos> substack;
        std::map<std::string,std::streampos> subs;

        std::stack<std::streampos> repstack;
        std::stack<int> reps;

        std::string replace_defs_with_vals(std::string line);
        std::string replace_rpns_with_vals(std::string line);;
        std::string replace_codepts_with_vals(std::string line);;
        std::string remove_comments(std::string line);
};

#endif
