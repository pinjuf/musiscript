#include "codepts.h"
#include "notes.h"
#include "util.h"
#include "wav.h"

#include <stdexcept>
#include <vector>
#include <sstream>
#include <math.h>
#include <iomanip>
#include <random>
#include <string.h>

int eval_codepointer(std::string input, std::string * output, Voice & vc) {
    std::vector<std::string> tokens;

    std::stringstream ss(input);
    std::string tok;
    while (getline(ss, tok, ' ')) {
        if (!tok.empty())
            tokens.push_back(tok);
    }

    if (tokens.size() == 0)
        return -1;

    if (!strcmp(tokens[0].c_str(), "numeq") ||
        !strcmp(tokens[0].c_str(), "==")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }

        if (a==b)
            *output = "true";
        else
            *output = "false";
    }

    else if (!strcmp(tokens[0].c_str(), "numgt") ||
        !strcmp(tokens[0].c_str(), ">")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }

        if (a>b)
            *output = "true";
        else
            *output = "false";
    }

    else if (!strcmp(tokens[0].c_str(), "numlt") ||
        !strcmp(tokens[0].c_str(), "<")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }

        if (a<b)
            *output = "true";
        else
            *output = "false";
    }

    else if (!strcmp(tokens[0].c_str(), "numgeqt") ||
        !strcmp(tokens[0].c_str(), ">=")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;

        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }

        if (a>=b)
            *output = "true";
        else
            *output = "false";
    }

    else if (!strcmp(tokens[0].c_str(), "numleqt") ||
        !strcmp(tokens[0].c_str(), "<=")) {
        double a, b;
        if (tokens.size() != 3)
            return -1;
        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }

        if (a<=b)
            *output = "true";
        else
            *output = "false";
    }

    else if (!strcmp(tokens[0].c_str(), "pi")) {
        *output = dtostr(M_PI);
    }

    else if (!strcmp(tokens[0].c_str(), "randint")) {
        if (tokens.size() != 3)
            return -1;
        int a, b;
        try {
            a = std::stoi(tokens[1]);
            b = std::stoi(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }
        int res = a + rand() % (b - a);
        *output = dtostr(res);
    }

    else if (!strcmp(tokens[0].c_str(), "randfloat")) {
        if (tokens.size() != 3)
            return -1;
        double a, b;
        try {
            a = std::stod(tokens[1]);
            b = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }
        double res = a + (b - a) * ((double)rand() / (double)RAND_MAX);
        *output = dtostr(res);
    }

    else if (!strcmp(tokens[0].c_str(), "currsamples")) {
        *output = dtostr(vc.counter);
    }

    else if (!strcmp(tokens[0].c_str(), "currtime")) {
        *output = dtostr((float)vc.counter / SAMPLING_RATE);
    }

    else if (!strcmp(tokens[0].c_str(), "currbeat")) {
        *output = dtostr(vc.speed * (float)vc.counter / SAMPLING_RATE);
    }

    else if (!strcmp(tokens[0].c_str(), "samplingrate")) {
        *output = dtostr(SAMPLING_RATE);
    }

    else if (!strcmp(tokens[0].c_str(), "mansample")) {
        if (tokens.size() != 3)
            return -1;
        double l, r;
        try {
            l = std::stod(tokens[1]);
            r = std::stod(tokens[2]);
        } catch (std::invalid_argument const&) {
            return -1;
        }
        int16_t ll, rr;
        ll = l * WavFile::def_amp;
        rr = r * WavFile::def_amp;
        StereoSample sample = {ll, rr};
        vc.samples.push_back(sample);
    }

    else if (!strcmp(tokens[0].c_str(), "freqof")) {
        if (tokens.size() != 2)
            return -1;

        double out = get_freq_by_name(tokens[1].c_str(), vc.transpose);
        if (out == -1)
            return -1;
        *output = dtostr(out);
    }

    else if (!strcmp(tokens[0].c_str(), "currpan")) {
        *output = dtostr(vc.pan);
    }

    else if (!strcmp(tokens[0].c_str(), "currvolume")) {
        *output = dtostr(vc.volume);
    }

    else if (!strcmp(tokens[0].c_str(), "currspeed")) {
        *output = dtostr(vc.speed);
    }

    else if (!strcmp(tokens[0].c_str(), "currtranspose")) {
        *output = dtostr(vc.transpose);
    }

    else if (!strcmp(tokens[0].c_str(), "chord")) {
        if (tokens.size() != 3)
            return -1;

        std::vector<std::string> out_notes = {tokens[1].c_str()};

        std::string base = tokens[1];

        if (!strcmp(tokens[2].c_str(), "maj")) { // TODO: expand
            out_notes.push_back(base + "++++");
            out_notes.push_back(base + "+++++++");
        } else if (!strcmp(tokens[2].c_str(), "min")) {
            out_notes.push_back(base + "+++");
            out_notes.push_back(base + "+++++++");
        }

        else
            log(LOG_WARNING, "Unknown chord type", true);

        *output = "";
        for (std::string note : out_notes) {
            *output += note + ",";
        }
    }

    return 0;
}
