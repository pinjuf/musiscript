#include "voices.h"
#include "effects.h"
#include "notes.h"
#include "sounds.h"
#include "wav.h"
#include "logging.h"

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

    size_t counter = 0;

    std::vector<StereoSample> samples = {};

    if (!file.is_open()) {
        log(LOG_ERROR, ("Could not open voice file " + std::string(filename)).c_str());
        return;
    }

    while(getline(file, line)) {
        for (auto const & x : defs) { // Replace all defined variables
            line = replace_all(line, x.first, x.second);
        }

        tokens = split_string(line, ' ');

        if (tokens.empty()) { // Empty line
            continue;
        }

        if (!strcmp(tokens[0].c_str(), "pan")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (pan)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (pan)");
            }
            try {
                pan = std::stod(tokens[1]);
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (pan)");
            }
        }
        else if (!strcmp(tokens[0].c_str(), "volume")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (volume)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (volume)");
            }
            try {
                volume = std::stod(tokens[1]);
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (volume)");
            }
        }
        else if (!strcmp(tokens[0].c_str(), "speed")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (speed)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (speed)");
            }
            try {
                speed = std::stod(tokens[1]);
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (speed)");
            }
        }
        else if (!strcmp(tokens[0].c_str(), "sound")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (sound)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (sound)");
            }

            sound = get_sound_by_name(tokens[1].c_str());

            if (sound == SILENCE) {
                try {
                    sound = (SOUNDS)std::stoi(tokens[1]);
                    log(LOG_WARNING, "Using a sound index is deprecated and may become problematic in the future. Please use the sound name instead.");
                } catch (std::invalid_argument) {}
            }

            if (sound == SILENCE) {
                log(LOG_ERROR, ("Unknown/silent sound name: " + tokens[1]).c_str());
            }
        }
        else if (!strcmp(tokens[0].c_str(), "transpose")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (transpose)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (transpose)");
            }
            try {
                transpose = std::stoi(tokens[1]);
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (transpose)");
            }
        }

        else if (!strcmp(tokens[0].c_str(), "effect")) {
            if (!strcmp(tokens[1].c_str(), "a")) { // 'a' for Add effect
                if (tokens.size() < 3) {
                    log(LOG_ERROR, "Not enough arguments (effect a)");
                    continue;
                }
                Effect effect = Effect();
                effect.start = counter;

                effect.effect = Effect::get_effect_by_name(tokens[2].c_str());

                if (effect.effect == NO_EFFECT) {
                    try {
                        effect.effect = (EFFECTS)std::stoi(tokens[2]);
                        log(LOG_WARNING, "Using an effect index is deprecated and may become problematic in the future. Use the corresponding effect name.");
                    } catch (std::invalid_argument) {} // No need, warning happens on NO_EFFECT check
                }

                if (effect.effect == NO_EFFECT) {
                    log(LOG_WARNING, ("Unknown/silent effect name: " + tokens[2]).c_str());
                }

                for (int i = 3; i < tokens.size(); i++) {
                    try {
                        effect.settings[i-3] = std::stod(tokens[i].c_str());
                    } catch (std::invalid_argument) {
                        log(LOG_ERROR, "Invalid argument (effect a)");
                        continue;
                    }
                }
                effects.push_back(effect); 
            }

            else if (!strcmp(tokens[1].c_str(), "c")) { // 'c' for Clear effect
                if (tokens.size() > 2) {
                    log(LOG_WARNING, "Ignored extra arguments (effect c)");
                }

                for (size_t i = 0; i < effects.size(); i++) {
                    effects[i].end = counter; // Keep the effect in the stack, but mark it as finished
                }
            }

            else if (!strcmp(tokens[1].c_str(), "r")) { // 'r' for Remove FIRST FOUND effect
                if (tokens.size() < 3) {
                    log(LOG_ERROR, "Not enough arguments (effect r)");
                    continue;
                }
                else if (tokens.size() > 3) {
                    log(LOG_WARNING, "Ignored extra arguments (effect r)");
                }

                EFFECTS r = Effect::get_effect_by_name(tokens[2].c_str());
                if (r == NO_EFFECT) {
                    try {
                        r = (EFFECTS)std::stoi(tokens[2]);
                        log(LOG_WARNING, "Using an effect index is deprecated and may become problematic in the future. Use the corresponding effect name.");
                    } catch (std::invalid_argument) {} // No need, warning happens on NO_EFFECT check
                }

                if (r == NO_EFFECT) {
                    log(LOG_WARNING, ("Unknown/silent effect name: " + tokens[2]).c_str());
                }

                for (size_t i = 0; i < effects.size(); i++) {
                    if (effects[i].effect == r) {
                        effects[i].end = counter;
                        break;
                    }
                }
            }

            else if (!strcmp(tokens[1].c_str(), "ra")) { // 'ra' for Remove ALL FOUND effects
                if (tokens.size() < 3) {
                    log(LOG_ERROR, "Not enough arguments (effect ra)");
                    continue;
                }
                else if (tokens.size() > 3) {
                    log(LOG_WARNING, "Ignored extra arguments (effect ra)");
                }

                EFFECTS r = Effect::get_effect_by_name(tokens[2].c_str());
                if (r == NO_EFFECT) {
                    try {
                        r = (EFFECTS)std::stoi(tokens[2]);
                        log(LOG_WARNING, "Using an effect index is deprecated and may become problematic in the future. Use the corresponding effect name.");
                    } catch (std::invalid_argument) {} // No need, warning happens on NO_EFFECT check
                }

                if (r == NO_EFFECT) {
                    log(LOG_WARNING, ("Unknown/silent effect name: " + tokens[2]).c_str());
                }

                for (size_t i = 0; i < effects.size(); i++) {
                    if (effects[i].effect == r) {
                        effects[i].end = counter;
                    }
                }
            }

            else {
                log(LOG_ERROR, "Unknown effect command");
            }
        }

        else if (!strcmp(tokens[0].c_str(), "n")) { // 'n' for Note
            if (tokens.size() < 3) {
                log(LOG_ERROR, "Not enough arguments (n)");
                continue;
            }
            std::vector<std::string> note_tokens = split_string(tokens[1], ',');
            std::vector<double> freqs;
            for (int i = 0; i < note_tokens.size(); i++) {
                if (note_tokens[i][0] == 'r') { // 'r' prefix: raw frequency input
                    try {
                        freqs.push_back(std::stod(note_tokens[i].c_str() + 1));
                    } catch (std::invalid_argument) {
                        log(LOG_ERROR, "Invalid argument (n)");
                        continue;
                    }

                } else {
                    double freq = get_freq_by_name((char*)note_tokens[i].c_str(), transpose);
                    if (freq >= 0) {
                        freqs.push_back(freq);
                    } else {
                        log(LOG_ERROR, "Invalid note name (n)");
                        continue;
                    }
                }
            }

            std::vector<std::string> duration_tokens = split_string(tokens[2], ',');
            std::vector<double> durations;
            for (int i = 0; i < duration_tokens.size(); i++) {
                try {
                    durations.push_back(std::stod(duration_tokens[i].c_str()));
                } catch (std::invalid_argument) {
                    log(LOG_ERROR, "Invalid argument (n)");
                    continue;
                }
            }

            StereoSample stereosample;

            for (int i = 0; i < durations[0]*SAMPLING_RATE/speed; i++) { // Note how the speed only affects the duration, not effects
                stereosample.l = 0;stereosample.r = 0;

                size_t baked_i = i; // Used mainly for vibrato
                for (int k = 0; k < effects.size(); k++) {
                    baked_i = effects[k].get_through_i_effect(baked_i, i, counter);
                }

                for (int j = 0; j < freqs.size(); j++) {
                    double baked_freq = freqs[j];
                    for (int k = 0; k < effects.size(); k++)
                        baked_freq = effects[k].get_through_freq_effect(freqs[j], i, counter);
                    // Get the amplitude, add the panning effect and divide by the number of frequencies to get the average amplitude
                    stereosample.l += get_sound_at_wavready(baked_i, baked_freq, (SOUNDS)sound) * (1 - pan) / freqs.size();
                    stereosample.r += get_sound_at_wavready(baked_i, baked_freq, (SOUNDS)sound) * pan / freqs.size();
                }

                for (int j = 0; j < effects.size(); j++)
                     effects[j].get_through_amp_effect(&stereosample, i, counter);

                stereosample.l *= volume;
                stereosample.r *= volume;

                samples.push_back(stereosample);

                counter++;
            }

            if (durations.size()>1) { // 2 or more durations were given, use the second one for a pause immediately after the first one
                if (durations.size()>2)
                    log(LOG_WARNING, "Ignored extra durations (n)");
                stereosample.l = 0;stereosample.r = 0;
                for (int i = 0; i < durations[1]*SAMPLING_RATE/speed; i++) {
                    samples.push_back(stereosample);
                    counter++;
                }
            }
        }

        else if (!strcmp(tokens[0].c_str(), "p")) { // 'p' for Pause
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (p)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (p)");
            }

            double duration;
            try {
                duration = std::stod(tokens[1].c_str());
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (p)");
                continue;
            }

            StereoSample stereosample = {0, 0};
            for (int i = 0; i < duration*SAMPLING_RATE/speed; i++) {
                samples.push_back(stereosample);
                counter++;
            }
        }    

        else if (!strcmp(tokens[0].c_str(), "w")) { // 'w' for Wait
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (w)");
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (w)");
            }

            double timestamp;
            try {
                timestamp = std::stod(tokens[1].c_str()) * SAMPLING_RATE / speed;
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (w)");
                continue;
            }

            while (counter < timestamp) {
                samples.push_back({0, 0});
                counter++;
            }
        }

        else if (!strcmp(tokens[0].c_str(), "def")) { // 'def' sets a definiton to be followed
            if (tokens.size() < 3) {
                log(LOG_ERROR, "Not enough arguments (def)");
                continue;
            }
            std::string value = "";
            for (int i = 2; i < tokens.size(); i++) {
                value += tokens[i];
                if (i != tokens.size() - 1)
                    value += " ";
            }
            defs["$"+tokens[1]] = value;
        }

        else if (!strcmp(tokens[0].c_str(), "sub")) { // 'sub' sets a subroutine
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (sub)");
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (sub)");
            }

            subs[tokens[1]] = file.tellg();

            // do 'getlines' until an 'endsub' is found
            int depth_counter = 1;
            std::string temp_line;
            while (getline(file, temp_line)) {
                std::vector<std::string> temp_tokens = split_string(temp_line, ' ');
                if (temp_tokens.size()) {
                    if (!strcmp(temp_tokens[0].c_str(), "endsub"))
                        depth_counter--;
                    else if (!strcmp(temp_tokens[0].c_str(), "sub"))
                        depth_counter++;
                }
                if (depth_counter == 0)
                    break;
            }
        }

        else if (!strcmp(tokens[0].c_str(), "endsub")) {
            if (tokens.size() > 1) {
                log(LOG_WARNING, "Ignored extra arguments (endsub)");
            }

            if (substack.size() > 0) {
                file.seekg(substack.top());
                substack.pop();
            } else {
                log(LOG_ERROR, "Found orphan 'endsub'");
            }
        }

        else if (!strcmp(tokens[0].c_str(), "call")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (call)");
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (call)");
            }

            if (subs.find(tokens[1]) == subs.end()) {
                log(LOG_ERROR, ("Subroutine '" + tokens[1] + "' not found").c_str());
                continue;
            }

            substack.push(file.tellg());
            file.seekg(subs[tokens[1]]);
        }

        else if (!strcmp(tokens[0].c_str(), "rep")) { // 'rep' repeats the following code n times
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (rep)");
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (rep)");
            }

            int n;
            try {
                n = atoi(tokens[1].c_str());
            } catch (std::invalid_argument) {
                log(LOG_ERROR, "Invalid argument (rep)");
                continue;
            }

            reps.push(n);
            repstack.push(file.tellg());
        }

        else if (!strcmp(tokens[0].c_str(), "endrep")) { // marks the end of a rep
            if (reps.empty()) {
                log(LOG_ERROR, "Found orphan 'endrep'");
                continue;
            }
            reps.top()--;
            if (reps.top()) {
                file.seekg(repstack.top());
            } else {
                reps.pop();
                repstack.pop();
            }
        }

        else if (!strcmp(tokens[0].c_str(), "end")) { // 'end' ends parsing
            break;
        }

        else if (tokens[0].c_str()[0] == '#') { // comment
            continue;
        }

        else {
            log(LOG_WARNING, ("Unknown command: " + tokens[0]).c_str());
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
