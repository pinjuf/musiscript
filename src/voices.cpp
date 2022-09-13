#include <cstdlib>
#include <string.h>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <iomanip>

#include "voices.h"
#include "effects.h"
#include "notes.h"
#include "sounds.h"
#include "wav.h"
#include "logging.h"
#include "math.h"
#include "codepts.h"
#include "util.h"
#include "config.h"

std::string Voice::replace_defs_with_vals(std::string line) {
    for (auto const & x : defs) { // Replace all defined variables
        line = replace_all(line, x.first, x.second);
    }
    return line;
}

std::string Voice::replace_infix_codepts_with_vals(std::string line) {
    // Codepointers and Infix notations may be nested.
    // Codepointers use curly braces, infix uses square brackets.
    // We need to replace them by their depth.
    // We do this by replacing codepointers/infixes that don't contain any other codepointers/infixes first,
    // then repeat that process until there are non left.
    // We should be able to recognize mismatched braces and log an error in that case.

    size_t blocks = 0;
    for (size_t i = 0; i < line.size(); i++) { // Count the number of "blocks"
        if (line[i] == ']' || line[i] == '}')
            blocks++;
    }

    for (size_t i = 0; i < blocks; i++) { // For each block, traverse the whole line
        size_t next_closing = line.find(']', 0); // Get first closing brace
        if (line.find('}', 0) < next_closing) {
            next_closing = line.find('}', 0);
        }

        if (next_closing == std::string::npos)
            break;

        // Get the last opening brace before the closing brace, rfind isn't working because i am too lazy
        size_t matching_opening = std::string::npos;
        for (size_t j = 0; j < next_closing; j++) {
            if (line[j] == '[' || line[j] == '{') {
                matching_opening = j;
            }
        }

        if (next_closing == std::string::npos || matching_opening == std::string::npos) {
            log(LOG_ERROR, "Missing codepointer/infix braces", true);
            return line;
        }

        if (line[next_closing] == '}') { // Codepointer
            std::string codept_name = line.substr(matching_opening + 1, next_closing - matching_opening - 1);
            std::string codept_result;

            if (eval_codepointer(codept_name, &codept_result, *this) < 0) {
                log(LOG_ERROR, "Invalid code pointer", true);
                return line;
            }

            line = replace_all(line, line.substr(matching_opening, next_closing - matching_opening + 1), codept_result);
        }

        else if (line[next_closing] == ']') { // Infix
            std::string infix = line.substr(matching_opening + 1, next_closing - matching_opening - 1);
            double infix_result;

            if (eval_infix(infix, &infix_result) < 0) {
                log(LOG_ERROR, "Invalid infix notation", true);
                return line;
            }

            std::stringstream ss;
            ss << std::setprecision(DIGIT_PREC) << infix_result;
            line = replace_all(line, line.substr(matching_opening, next_closing - matching_opening + 1), ss.str());
        }
    }

    return line;
}

std::string Voice::remove_comments(std::string line) {
    size_t start_pos = 0;
    while((start_pos = line.find('#', start_pos)) != std::string::npos) {
        line.erase(start_pos);
    }
    return line;
}

void Voice::read_from_file(const char * filename, std::vector<StereoSample> * outsamples) {
    std::ifstream file(filename);
    std::string line;
    std::vector<std::string> tokens;

    counter = 0;
    size_t line_num;

    samples = {};

    if (!file.is_open()) {
        log(LOG_ERROR, ("Could not open voice file " + std::string(filename)).c_str());
        return;
    }

    set_line_num_ptr(&line_num); // Connect to logging system

    while(getline(file, line)) {
#if DO_LOGLINENUMBER
        line_num = get_current_line(file); // Update line number for logging system, rather inefficient but simple
#endif

        line = strip_line(line);
        line = remove_comments(line);

        line = replace_defs_with_vals(line);

        line = replace_infix_codepts_with_vals(line);

        tokens = split_string(line, ' ');

        if (tokens.empty()) { // Empty line
            continue;
        }

        if (!strcmp(tokens[0].c_str(), "pan")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (pan)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (pan)", true);
            }
            try {
                pan = std::stod(tokens[1]);
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (pan)", true);
            }
        }
        else if (!strcmp(tokens[0].c_str(), "volume")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (volume)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (volume)", true);
            }
            try {
                volume = std::stod(tokens[1]);
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (volume)", true);
            }
        }
        else if (!strcmp(tokens[0].c_str(), "speed")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (speed)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (speed)", true);
            }
            try {
                speed = std::stod(tokens[1]);
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (speed)", true);
            }
        }
        else if (!strcmp(tokens[0].c_str(), "sound")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (sound)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (sound)", true);
            }

            sound = get_sound_by_name(tokens[1].c_str());

            if (sound == SILENCE) {
                try {
                    sound = (SOUNDS)std::stoi(tokens[1]);
                    log(LOG_WARNING, "Using a sound index is deprecated and may become problematic in the future. Please use the sound name instead.", true);
                } catch (std::invalid_argument const&) {}
            }

            if (sound == SILENCE) {
                log(LOG_ERROR, ("Unknown/silent sound name: " + tokens[1]).c_str(), true);
            }
        }
        else if (!strcmp(tokens[0].c_str(), "transpose")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (transpose)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (transpose)", true);
            }
            try {
                transpose = std::stoi(tokens[1]);
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (transpose)", true);
            }
        }

        else if (!strcmp(tokens[0].c_str(), "effect")) {
            if (!strcmp(tokens[1].c_str(), "a")) { // 'a' for Add effect
                if (tokens.size() < 3) {
                    log(LOG_ERROR, "Not enough arguments (effect a)", true);
                    continue;
                }
                Effect effect = Effect();
                effect.start = counter;

                effect.effect = Effect::get_effect_by_name(tokens[2].c_str());

                if (effect.effect == NO_EFFECT) {
                    try {
                        effect.effect = (EFFECTS)std::stoi(tokens[2]);
                        log(LOG_WARNING, "Using an effect index is deprecated and may become problematic in the future. Use the corresponding effect name.", true);
                    } catch (std::invalid_argument const&) {} // No need, warning happens on NO_EFFECT check
                }

                if (effect.effect == NO_EFFECT) {
                    log(LOG_WARNING, ("Unknown/silent effect name: " + tokens[2]).c_str(), true);
                }

                for (size_t i = 3; i < tokens.size(); i++) {
                    try {
                        effect.settings[i-3] = std::stod(tokens[i].c_str());
                    } catch (std::invalid_argument const&) {
                        log(LOG_ERROR, "Invalid argument (effect a)", true);
                        continue;
                    }
                }
                effects.push_back(effect); 
            }

            else if (!strcmp(tokens[1].c_str(), "c")) { // 'c' for Clear effect
                if (tokens.size() > 2) {
                    log(LOG_WARNING, "Ignored extra arguments (effect c)", true);
                }

                for (size_t i = 0; i < effects.size(); i++) {
                    if (effects[i].end > counter) // Has the effect already been ended?
                        effects[i].end = counter; // Keep the effect in the stack, but mark it as finished
                }
            }

            else if (!strcmp(tokens[1].c_str(), "r")) { // 'r' for Remove FIRST FOUND effect
                if (tokens.size() < 3) {
                    log(LOG_ERROR, "Not enough arguments (effect r)", true);
                    continue;
                }
                else if (tokens.size() > 3) {
                    log(LOG_WARNING, "Ignored extra arguments (effect r)", true);
                }

                EFFECTS r = Effect::get_effect_by_name(tokens[2].c_str());
                if (r == NO_EFFECT) {
                    try {
                        r = (EFFECTS)std::stoi(tokens[2]);
                        log(LOG_WARNING, "Using an effect index is deprecated and may become problematic in the future. Use the corresponding effect name.", true);
                    } catch (std::invalid_argument const&) {} // No need, warning happens on NO_EFFECT check
                }

                if (r == NO_EFFECT) {
                    log(LOG_WARNING, ("Unknown/silent effect name: " + tokens[2]).c_str(), true);
                }

                for (size_t i = 0; i < effects.size(); i++) {
                    if (effects[i].effect == r && effects[i].end > counter) {
                        effects[i].end = counter;
                        break;
                    }
                }
            }

            else if (!strcmp(tokens[1].c_str(), "ra")) { // 'ra' for Remove ALL FOUND effects
                if (tokens.size() < 3) {
                    log(LOG_ERROR, "Not enough arguments (effect ra)", true);
                    continue;
                }
                else if (tokens.size() > 3) {
                    log(LOG_WARNING, "Ignored extra arguments (effect ra)", true);
                }

                EFFECTS r = Effect::get_effect_by_name(tokens[2].c_str());
                if (r == NO_EFFECT) {
                    try {
                        r = (EFFECTS)std::stoi(tokens[2]);
                        log(LOG_WARNING, "Using an effect index is deprecated and may become problematic in the future. Use the corresponding effect name.", true);
                    } catch (std::invalid_argument const&) {} // No need, warning happens on NO_EFFECT check
                }

                if (r == NO_EFFECT) {
                    log(LOG_WARNING, ("Unknown/silent effect name: " + tokens[2]).c_str(), true);
                }

                for (size_t i = 0; i < effects.size(); i++) {
                    if (effects[i].effect == r && effects[i].end > counter) {
                        effects[i].end = counter;
                    }
                }
            }

            else {
                log(LOG_ERROR, "Unknown effect command", true);
            }
        }

        else if (!strcmp(tokens[0].c_str(), "n")) { // 'n' for Note
            if (tokens.size() < 3) {
                log(LOG_ERROR, "Not enough arguments (n)", true);
                continue;
            }
            std::vector<std::string> note_tokens = split_string(tokens[1], ',');
            std::vector<double> freqs;
            for (size_t i = 0; i < note_tokens.size(); i++) {
                if (note_tokens[i][0] == 'r') { // 'r' prefix: raw frequency input
                    try {
                        freqs.push_back(std::stod(note_tokens[i].c_str() + 1));
                    } catch (std::invalid_argument const&) {
                        log(LOG_ERROR, "Invalid argument (n)", true);
                        continue;
                    }

                } else {
                    double freq = get_freq_by_name(note_tokens[i].c_str(), transpose);
                    if (freq >= 0) {
                        freqs.push_back(freq);
                    } else {
                        log(LOG_ERROR, "Invalid note name (n)", true);
                        continue;
                    }
                }
            }

            std::vector<std::string> duration_tokens = split_string(tokens[2], ',');
            std::vector<double> durations;
            for (size_t i = 0; i < duration_tokens.size(); i++) {
                try {
                    durations.push_back(std::stod(duration_tokens[i].c_str()));
                } catch (std::invalid_argument const&) {
                    log(LOG_ERROR, "Invalid argument (n)", true);
                    continue;
                }
            }

            StereoSample stereosample;

            for (int i = 0; i < durations[0]*SAMPLING_RATE/speed; i++) { // Note how the speed only affects the duration, not effects
                stereosample.l = 0;stereosample.r = 0;

                size_t baked_i = i; // Used mainly for vibrato
                for (size_t k = 0; k < effects.size(); k++) {
                    baked_i = effects[k].get_through_i_effect(baked_i, i, counter);
                }

                for (size_t j = 0; j < freqs.size(); j++) {
                    double baked_freq = freqs[j];
                    for (size_t k = 0; k < effects.size(); k++)
                        baked_freq = effects[k].get_through_freq_effect(freqs[j], i, counter);
                    // Get the amplitude, add the panning effect and divide by the number of frequencies to get the average amplitude
                    stereosample.l += get_sound_at_wavready(baked_i, baked_freq, (SOUNDS)sound) * (1 - pan) / freqs.size();
                    stereosample.r += get_sound_at_wavready(baked_i, baked_freq, (SOUNDS)sound) * pan / freqs.size();
                }

                stereosample.l *= volume;
                stereosample.r *= volume;

                for (size_t j = 0; j < effects.size(); j++)
                     effects[j].get_through_amp_effect(&stereosample, i, counter);

                samples.push_back(stereosample);

                counter++;
            }

            if (durations.size()>1) { // 2 or more durations were given, use the second one for a pause immediately after the first one
                if (durations.size()>2)
                    log(LOG_WARNING, "Ignored extra durations (n)", true);
                stereosample.l = 0;stereosample.r = 0;
                for (int i = 0; i < durations[1]*SAMPLING_RATE/speed; i++) {
                    samples.push_back(stereosample);
                    counter++;
                }
            }
        }

        else if (!strcmp(tokens[0].c_str(), "p")) { // 'p' for Pause
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (p)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (p)", true);
            }

            double duration;
            try {
                duration = std::stod(tokens[1].c_str());
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (p)", true);
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
                log(LOG_ERROR, "Not enough arguments (w)", true);
                continue;
            }
            else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (w)", true);
            }

            double timestamp;
            try {
                timestamp = std::stod(tokens[1].c_str()) * SAMPLING_RATE / speed;
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (w)", true);
                continue;
            }

            while (counter < timestamp) {
                samples.push_back({0, 0});
                counter++;
            }
        }

        else if (!strcmp(tokens[0].c_str(), "def")) { // 'def' sets a definiton to be followed
            if (tokens.size() < 3) {
                log(LOG_ERROR, "Not enough arguments (def)", true);
                continue;
            }
            std::string value = "";
            for (size_t i = 2; i < tokens.size(); i++) {
                value += tokens[i];
                if (i != tokens.size() - 1)
                    value += " ";
            }
            defs["$"+tokens[1]] = value;
        }

        else if (!strcmp(tokens[0].c_str(), "undef")) { // 'undef' deletes a variable
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (undef)", true);
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (undef)", true);
            }

            if (defs.find("$"+tokens[1]) == defs.end()) {
                log(LOG_ERROR, "Undefined variable (undef)", true);
                continue;
            }

            defs.erase("$"+tokens[1]);
        }

        else if (!strcmp(tokens[0].c_str(), "sub")) { // 'sub' sets a subroutine
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (sub)", true);
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (sub)", true);
            }

            subs[tokens[1]] = file.tellg();

            // do 'getlines' until an 'endsub' is found
            int depth_counter = 1;
            bool found = false;
            std::string temp_line;
            while (getline(file, temp_line)) {
                std::vector<std::string> temp_tokens = split_string(temp_line, ' ');
                if (temp_tokens.size()) {
                    if (!strcmp(temp_tokens[0].c_str(), "endsub"))
                        depth_counter--;
                    else if (!strcmp(temp_tokens[0].c_str(), "sub"))
                        depth_counter++;
                }
                if (depth_counter == 0) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                log(LOG_ERROR, "Reached EOF while reading 'sub' definition", true);
                continue;
            }
        }

        else if (!strcmp(tokens[0].c_str(), "endsub")) {
            if (tokens.size() > 1) {
                log(LOG_WARNING, "Ignored extra arguments (endsub)", true);
            }

            if (substack.size() > 0) {
                file.seekg(substack.top());
                substack.pop();
            } else {
                log(LOG_ERROR, "Found orphan 'endsub'", true);
            }
        }

        else if (!strcmp(tokens[0].c_str(), "call")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (call)", true);
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (call)", true);
            }

            if (subs.find(tokens[1]) == subs.end()) {
                log(LOG_ERROR, ("Subroutine '" + tokens[1] + "' not found").c_str(), true);
                continue;
            }

            substack.push(file.tellg());
            file.seekg(subs[tokens[1]]);
        }

        else if (!strcmp(tokens[0].c_str(), "rep")) { // 'rep' repeats the following code n times
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (rep)", true);
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (rep)", true);
            }

            int n;
            try {
                n = atoi(tokens[1].c_str());
            } catch (std::invalid_argument const&) {
                log(LOG_ERROR, "Invalid argument (rep)", true);
                continue;
            }

            reps.push(n);
            repstack.push(file.tellg());
        }

        else if (!strcmp(tokens[0].c_str(), "endrep")) { // marks the end of a rep
            if (reps.empty()) {
                log(LOG_ERROR, "Found orphan 'endrep'", true);
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

        else if (!strcmp(tokens[0].c_str(), "echo")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (echo)", true);
                continue;
            }
            std::string l = "(echo) ";
            l += line.c_str()+sizeof("echo");
            log(LOG_INFO, l.c_str(), true);
        }

        else if (!strcmp(tokens[0].c_str(), "jump")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (jump)", true);
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (jump)", true);
            }

            file.seekg(0);
            std::string temp_line;
            bool found = false;
            while (getline(file, temp_line)) {
                std::vector<std::string> temp_tokens = split_string(temp_line, ' ');
                if (temp_tokens.size() < 2)
                    continue;
                if (!strcmp(temp_tokens[0].c_str(), "label") && !strcmp(temp_tokens[1].c_str(), tokens[1].c_str())) {
                    found = true;
                    break;
                }
            }

            if (!found)
                log(LOG_WARNING, "Jump target not found (jump)", true);
        }

        else if (!strcmp(tokens[0].c_str(), "label")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (label)", true);
                continue;
            } else if (tokens.size() > 2) {
                log(LOG_WARNING, "Ignored extra arguments (label)", true);
            }
        }

        else if (!strcmp(tokens[0].c_str(), "if")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (if)", true);
                continue;
            }

            bool condition = false; // wrong by default

            if (eval_infix_logical(line.c_str()+sizeof("if"), &condition) < 0) {
                log(LOG_ERROR, "Invalid condition (if)", true);
                continue;
            }
            ifs.push(condition);

            if (condition == true)
                continue;

            int depth_counter = 0;
            std::streampos prev = file.tellg();
            std::string temp_line;
            while (getline(file, temp_line)) {
                std::vector<std::string> temp_tokens = split_string(temp_line, ' ');
                if (temp_tokens.empty()) {
                    prev = file.tellg();
                    continue;
                }

                if (depth_counter == 0 &&
                        (!strcmp(temp_tokens[0].c_str(), "else") ||
                         !strcmp(temp_tokens[0].c_str(), "elif") ||
                         !strcmp(temp_tokens[0].c_str(), "endif"))) {
                    file.seekg(prev);
                    break;
                }

                if (!strcmp(temp_tokens[0].c_str(), "if"))
                    depth_counter++;
                else if (!strcmp(temp_tokens[0].c_str(), "endif"))
                    depth_counter--;

                prev = file.tellg();
            }

            file.seekg(prev); // Let next mainloop iteration handle the endif/elif/else
        }

        else if (!strcmp(tokens[0].c_str(), "elif")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "Not enough arguments (elif)", true);
                continue;
            }

            if (ifs.empty()) {
                log(LOG_ERROR, "Found orphan 'elif'", true);
                continue;
            }

            if (!ifs.top()) { // Last 'if' was not run
                bool condition = false; // wrong by default
                if (eval_infix_logical(line.c_str()+sizeof("elif"), &condition) < 0) { // Note: sizeof("elif") includes the space (that's actually the null terminator)
                    log(LOG_ERROR, "Invalid condition (elif)", true);
                    continue;
                }

                if (condition) {
                    ifs.pop();
                    ifs.push(true); // for later 'elif's/'else's
                    continue;
                }
            }

            // If we are here, either the last 'if' was run, or the elif was not run

            int depth_counter = 0;
            std::streampos prev = file.tellg();
            std::string temp_line;
            while (getline(file, temp_line)) {
                std::vector<std::string> temp_tokens = split_string(temp_line, ' ');
                if (temp_tokens.empty()) {
                    prev = file.tellg();
                    continue;
                }

                if (depth_counter == 0 &&
                        (!strcmp(temp_tokens[0].c_str(), "else") ||
                         !strcmp(temp_tokens[0].c_str(), "elif") ||
                         !strcmp(temp_tokens[0].c_str(), "endif"))) {
                    file.seekg(prev);
                    break;
                }

                if (!strcmp(temp_tokens[0].c_str(), "if"))
                    depth_counter++;
                else if (!strcmp(temp_tokens[0].c_str(), "endif"))
                    depth_counter--;

                prev = file.tellg();
            }
        }

        else if (!strcmp(tokens[0].c_str(), "else")) {
            if (tokens.size() > 1) {
                log(LOG_WARNING, "Ignored extra arguments (else)", true);
            }

            if (ifs.empty()) {
                log(LOG_ERROR, "Found orphan 'else'", true);
                continue;
            }

            if (!ifs.top()) { // Last 'if' was not run, continue parsing as usual
                continue;
            }

            int depth_counter = 0;
            std::streampos prev = file.tellg();
            std::string temp_line;
            while (getline(file, temp_line)) {
                std::vector<std::string> temp_tokens = split_string(temp_line, ' ');
                if (temp_tokens.empty()) {
                    prev = file.tellg();
                    continue;
                }

                if (depth_counter == 0 &&
                        (!strcmp(temp_tokens[0].c_str(), "endif"))) { // 'else' is always last in an 'if' block, so only look for endif
                    file.seekg(prev);
                    break;
                }

                if (!strcmp(temp_tokens[0].c_str(), "if"))
                    depth_counter++;
                else if (!strcmp(temp_tokens[0].c_str(), "endif"))
                    depth_counter--;

                prev = file.tellg();
            }

            file.seekg(prev);
        }

        else if (!strcmp(tokens[0].c_str(), "endif")) {
            if (tokens.size() > 1) {
                log(LOG_WARNING, "Ignored extra arguments (endif)", true);
            }

            if (ifs.empty()) {
                log(LOG_ERROR, "Found orphan 'endif'", true);
                continue;
            }

            ifs.pop();
        }

        else if (!strcmp(tokens[0].c_str(), "end")) { // 'end' ends parsing
            break;
        }

        else {
            log(LOG_WARNING, ("Unknown command: " + tokens[0]).c_str(), true);
        }
    }

    if (!ifs.empty()) {
        log(LOG_ERROR, "Reached EOF while reading 'if'", false);
    }
    if (!reps.empty()) {
        log(LOG_ERROR, "Reached EOF while reading 'rep'", false);
    }

    for (size_t i = 0; i < effects.size(); i++)
        effects[i].get_through_buffer_effect(&samples);

    for (size_t i = 0; i < samples.size(); i++) { // Add the samples to the output vector
        if (i < outsamples->size()) {
            outsamples->at(i).l += samples[i].l;
            outsamples->at(i).r += samples[i].r;
        } else {
            outsamples->push_back(samples[i]);
        }
    }
}

void Voice::preprocess_file(const char * path, const char * outpath) {
    std::ifstream file(path);
    if (!file.is_open()) {
        log(LOG_FATAL, "Failed to open voice file for preprocessing", false);
        return;
    }

    std::ofstream outfile(outpath);
    if (!outfile.is_open()) {
        log(LOG_FATAL, "Failed to open output file for preprocessing", false);
        return;
    }

    std::string line;
    std::vector<std::string> tokens;
    while (getline(file, line)) {
        line = strip_line(line);

        if (line.empty()) {
            outfile << line << std::endl;
            continue;
        }
        if (!(line.find("%") == 0)) {
            outfile << line << std::endl;
            continue;
        }

        line.erase(0, 1); // Remove the "%"
        
        tokens = split_string(line, ' ');

        if (tokens.empty()) {
            outfile << line << std::endl;
            continue;
        }

        if (!strcmp(tokens[0].c_str(), "include")) {
            if (tokens.size() < 2) {
                log(LOG_ERROR, "(PP) Not enough arguments (include)");
                continue;
            }

            preprocess_file(((std::string)DATA_DIR"/"+tokens[1]).c_str(), ((std::string)DATA_DIR"/"+tokens[1]+PREPROCESSED_VOICE_SUFFIX).c_str());

            std::ifstream incfile(((std::string)DATA_DIR"/"+tokens[1]+PREPROCESSED_VOICE_SUFFIX).c_str());
            if (!incfile.is_open()) {
                log(LOG_ERROR, "(PP) Failed to open included file (after preprocessing)");
                continue;
            }

            std::string inc_line;
            while (getline(incfile, inc_line)) {
                outfile << inc_line << std::endl;
            }
        }

        else
            outfile << line << std::endl;
    }

    file.close();
    outfile.close();
}
