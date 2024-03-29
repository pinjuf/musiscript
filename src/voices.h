#pragma once

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
        void preprocess_file(const char * path, const char * out_path);

        double pan    = 0.5;
        double volume = 1;
        double speed  = 1;
        double transpose = 0;
        int sound     = SILENCE;

        void read_from_file(const char * filename, std::vector<StereoSample> * outsamples);

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
