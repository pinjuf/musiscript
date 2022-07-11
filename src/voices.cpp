#include "voices.h"
#include "notes.h"
#include "sounds.h"

std::vector<std::string> split_string(std::string str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string tok;
    while(getline(ss, tok, delimiter)) {
        internal.push_back(tok);
    }
    return internal;
}

void Voice::read_from_file(char * filename, std::vector<StereoSample> * outsamples) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> tokens;

    uint64_t counter = 0;

    while(getline(file, line)) {
        tokens = split_string(line, ' ');

        if (tokens.size() == 0) {
            continue;
        }

        if (!strcmp(tokens[0].c_str(), "pan"))
            pan = atof(tokens[1].c_str());
        else if (!strcmp(tokens[0].c_str(), "volume"))
            volume = atof(tokens[1].c_str());
        else if (!strcmp(tokens[0].c_str(), "speed"))
            speed = atof(tokens[1].c_str());
        else if (!strcmp(tokens[0].c_str(), "sound"))
            sound = atoi(tokens[1].c_str());

        else if (!strcmp(tokens[0].c_str(), "n")) {
            std::vector<std::string> note_tokens = split_string(tokens[1], ',');
            std::vector<double> freqs;
            for (int i = 0; i < note_tokens.size(); i++) {
                freqs.push_back(get_freq_by_name((char*)note_tokens[i].c_str()));
            }

            double duration = atof(tokens[2].c_str());

            StereoSample stereosample;

            for (int i = 0; i < duration*44100/speed; i++) {
                stereosample.l = 0;stereosample.r = 0;

                for (int j = 0; j < freqs.size(); j++) {
                    stereosample.l += get_sound_at_wavready(i, freqs[j], (SOUNDS)sound, volume) * (1 - pan) / freqs.size();
                    stereosample.r += get_sound_at_wavready(i, freqs[j], (SOUNDS)sound, volume) * pan / freqs.size();
                }

                if (counter < outsamples->size()) {
                    outsamples->at(counter).l += stereosample.l;
                    outsamples->at(counter).r += stereosample.r;
                } else {
                    outsamples->push_back(stereosample);
                }

                counter++;
            }
        }

        else if (!strcmp(tokens[0].c_str(), "p")) {
            double duration = atof(tokens[1].c_str());

            StereoSample stereosample;

            for (int i = 0; i < duration*44100/speed; i++) {
                stereosample.l = get_sound_at_wavready(0, 0, SILENCE, volume);
                stereosample.r = get_sound_at_wavready(0, 0, SILENCE, volume);

                if (counter < outsamples->size()) {
                    outsamples->at(counter).l += stereosample.l;
                    outsamples->at(counter).r += stereosample.r;
                } else {
                    outsamples->push_back(stereosample);
                }


                counter++;
            }
        }    
    }
}
