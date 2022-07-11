#include "voices.h"
#include "effects.h"

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
    effects.clear();

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
        else if (!strcmp(tokens[0].c_str(), "transpose"))
            transpose = atoi(tokens[1].c_str());

        else if (!strcmp(tokens[0].c_str(), "effect")) {
            if (!strcmp(tokens[1].c_str(), "a")) {
                Effect effect = Effect();
                effect.effect = (EFFECTS)atoi(tokens[2].c_str());
                for (int i = 3; i < tokens.size(); i++) {
                    effect.settings[i-3] = atof(tokens[i].c_str());
                }
                effects.push_back(effect); 
            }
            if (!strcmp(tokens[0].c_str(), "c"))
                effects.clear();
        }

        else if (!strcmp(tokens[0].c_str(), "n")) {
            std::vector<std::string> note_tokens = split_string(tokens[1], ',');
            std::vector<double> freqs;
            for (int i = 0; i < note_tokens.size(); i++) {
                freqs.push_back(get_freq_by_name((char*)note_tokens[i].c_str(), transpose));
            }

            std::vector<std::string> duration_tokens = split_string(tokens[2], '+');
            std::vector<double> durations;
            for (int i = 0; i < duration_tokens.size(); i++) {
                durations.push_back(atof(duration_tokens[i].c_str()));
            }

            StereoSample stereosample;

            for (int i = 0; i < durations[0]*SAMPLING_RATE/speed; i++) {
                stereosample.l = 0;stereosample.r = 0;

                for (int j = 0; j < freqs.size(); j++) {
                    stereosample.l += get_sound_at_wavready(i, freqs[j], (SOUNDS)sound) * (1 - pan) / freqs.size();
                    stereosample.r += get_sound_at_wavready(i, freqs[j], (SOUNDS)sound) * pan / freqs.size();
                }

                for (int j = 0; j < effects.size(); j++)
                     effects[j].get_through_effect(&stereosample, i);

                stereosample.l *= volume;
                stereosample.r *= volume;

                if (counter < outsamples->size()) {
                    outsamples->at(counter).l += stereosample.l;
                    outsamples->at(counter).r += stereosample.r;
                } else {
                    outsamples->push_back(stereosample);
                }

                counter++;
            }

            if (durations.size()==2) {
                for (int i = 0; i < durations[1]*SAMPLING_RATE/speed; i++) {
                    stereosample.l = 0;stereosample.r = 0;

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

        else if (!strcmp(tokens[0].c_str(), "p")) {
            double duration = atof(tokens[1].c_str());

            StereoSample stereosample;

            for (int i = 0; i < duration*SAMPLING_RATE/speed; i++) {
                stereosample.l = 0;stereosample.r = 0;

                if (counter < outsamples->size()) {
                    outsamples->at(counter).l += stereosample.l;
                    outsamples->at(counter).r += stereosample.r;
                } else {
                    outsamples->push_back(stereosample);
                }


                counter++;
            }
        }    

        else if (!strcmp(tokens[0].c_str(), "end")) {
            break;
        }
    }
}
