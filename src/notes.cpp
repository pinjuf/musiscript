#include <stdexcept>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <string>

#include "notes.h"

double get_freq_by_name(char * name, int transpose) {
    double out;

    char base = name[0];
    if (isupper(base)) base += 32; // Lowercae the base
    
    size_t len = 0;
    for (;name[len];len++);
    if (len < 2)
        return -1;

    for (int i = 2; name[i]; i++) {
        switch (name[i]) {
            case '+':
            case '#':
                transpose++;
                break;
            case '-':
            case 'b':
                transpose--;
                break;
            default:
                transpose;
        }
    }

    int octave;
    try {
        octave = std::stoi(name+1) - 4; // Reference frequency is 4 octaves up
    } catch (std::invalid_argument) {
        return -1;
    }

    switch (base) {
        case 'a':
            out = A4_FREQ * pow(HALFTONE_STEP, 0+transpose);
            break;
        case 'h':
        case 'b':
            out = A4_FREQ * pow(HALFTONE_STEP, 2+transpose);
            break;
        case 'c':
            out = A4_FREQ * pow(HALFTONE_STEP, 3+transpose);
            octave--;
            break;
        case 'd':
            out = A4_FREQ * pow(HALFTONE_STEP, 5+transpose);
            octave--;
            break;
        case 'e':
            out = A4_FREQ * pow(HALFTONE_STEP, 7+transpose);
            octave--;
            break;
        case 'f':
            out = A4_FREQ * pow(HALFTONE_STEP, 8+transpose);
            octave--;
            break;
        case 'g':
            out = A4_FREQ * pow(HALFTONE_STEP, 10+transpose);
            octave--;
            break;
        default:
            return -1;
    }

    if (octave>0) out *= 1<<octave;
    if (octave<0) out /= 1<<-octave;

    return out;
}
