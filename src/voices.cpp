#include "voices.h"
#include "effects.h"
#include "wav.h"

std::vector<std::string> split_string(std::string str, char delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string tok;
    while(getline(ss, tok, delimiter)) {
        if (!tok.empty())
            internal.push_back(tok);
    }
    return internal;
}

std::string replace_all(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void Voice::read_from_file(char * filename, std::vector<StereoSample> * outsamples) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> tokens;
    effects.clear();
    defs.clear();

    uint64_t counter = 0;

    std::vector<StereoSample> samples = {};

    while(getline(file, line)) {
        for (auto const & x : defs) {
            line = replace_all(line, x.first, x.second);
        }

        tokens = split_string(line, ' ');

        if (tokens.empty()) { // Empty line
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
            if (!strcmp(tokens[1].c_str(), "a")) { // 'a' for Add effect
                Effect effect = Effect();
                effect.effect = (EFFECTS)atoi(tokens[2].c_str());
                for (int i = 3; i < tokens.size(); i++) {
                    effect.settings[i-3] = atof(tokens[i].c_str());
                }
                effects.push_back(effect); 
            }
            if (!strcmp(tokens[0].c_str(), "c")) // 'c' for Clear effect
                effects.clear();
        }

        else if (!strcmp(tokens[0].c_str(), "n")) { // 'n' for Note
            std::vector<std::string> note_tokens = split_string(tokens[1], ',');
            std::vector<double> freqs;
            for (int i = 0; i < note_tokens.size(); i++) {
                if (note_tokens[i][0] == 'r') { // 'r' prefix: raw frequency input
                    freqs.push_back(atof(note_tokens[i].c_str() + 1));
                } else {
                    freqs.push_back(get_freq_by_name((char*)note_tokens[i].c_str(), transpose));
                }
            }

            std::vector<std::string> duration_tokens = split_string(tokens[2], '+');
            std::vector<double> durations;
            for (int i = 0; i < duration_tokens.size(); i++) {
                durations.push_back(atof(duration_tokens[i].c_str()));
            }

            StereoSample stereosample;

            for (int i = 0; i < durations[0]*SAMPLING_RATE/speed; i++) { // Note how the speed only affects the duration, not effects
                stereosample.l = 0;stereosample.r = 0;

                uint64_t baked_i = i; // Used mainly for vibrato
                for (int k = 0; k < effects.size(); k++) {
                    baked_i = effects[k].get_through_i_effect(baked_i, i);
                }

                for (int j = 0; j < freqs.size(); j++) {
                    double baked_freq = freqs[j];
                    for (int k = 0; k < effects.size(); k++)
                        baked_freq = effects[k].get_through_freq_effect(freqs[j], i);
                    // Get the amplitude, add the panning effect and divide by the number of frequencies to get the average amplitude
                    stereosample.l += get_sound_at_wavready(baked_i, baked_freq, (SOUNDS)sound) * (1 - pan) / freqs.size();
                    stereosample.r += get_sound_at_wavready(baked_i, baked_freq, (SOUNDS)sound) * pan / freqs.size();
                }

                for (int j = 0; j < effects.size(); j++)
                     effects[j].get_through_amp_effect(&stereosample, i);

                stereosample.l *= volume;
                stereosample.r *= volume;

                samples.push_back(stereosample);

                counter++;
            }

            if (durations.size()==2) { // 2 durations were given, use the second one for a pause immediately after the first one
                stereosample.l = 0;stereosample.r = 0;
                for (int i = 0; i < durations[1]*SAMPLING_RATE/speed; i++) {
                    samples.push_back(stereosample);
                    counter++;
                }
            }
        }

        else if (!strcmp(tokens[0].c_str(), "p")) { // 'p' for Pause
            double duration = atof(tokens[1].c_str());

            StereoSample stereosample = {0, 0};

            for (int i = 0; i < duration*SAMPLING_RATE/speed; i++) {
                samples.push_back(stereosample);
                counter++;
            }
        }    

        else if (!strcmp(tokens[0].c_str(), "w")) { // 'w' for Wait
            double timestamp = atof(tokens[1].c_str()) * SAMPLING_RATE / speed;

            while (counter < timestamp) {
                samples.push_back({0, 0});
                counter++;
            }
        }

        else if (!strcmp(tokens[0].c_str(), "def")) { // 'def' sets a definiton to be followed
            std::string value = "";
            for (int i = 2; i < tokens.size(); i++) {
                value += tokens[i];
                if (i != tokens.size() - 1)
                    value += " ";
            }
            defs["$"+tokens[1]] = value;
        }

        else if (!strcmp(tokens[0].c_str(), "end")) { // 'end' ends parsing
            break;
        }

        // TODO: Add comments & logging system

        else {
            std::cerr << "[!] Unknown command: " << tokens[0] << std::endl;
        }
    }

    for (int i = 0; i < effects.size(); i++)
        effects[i].get_through_buffer_effect(&samples);

    for (int i = 0; i < samples.size(); i++) { // Add the samples to the output vector
        if (i < outsamples->size()) {
            outsamples->at(i).l += samples[i].l;
            outsamples->at(i).r += samples[i].r;
        } else {
            outsamples->push_back(samples[i]);
        }
    }
}
