#include "notes.h"

double get_freq_by_name(char * name) {
    char base = name[0];
    if (isupper(base)) base += 32;

    double out;
    int mod = 0;

    for (int i = 0; name[i]; i++) {
        switch (name[i]) {
            case '+':
            case '#':
                mod = 1;
                break;
            case '-':
            case 'b':
                mod = -1;
                break;
            default:
                mod = 0;
        }
        if (mod) break;
    }

    int octave = atoi(name+1) - 4;

    switch (base) {
        case 'a':
            out = A4_FREQ * pow(HALFTONE_STEP, 0+mod);
            break;
        case 'h':
        case 'b':
            out = A4_FREQ * pow(HALFTONE_STEP, 2+mod);
            break;
        case 'c':
            out = A4_FREQ * pow(HALFTONE_STEP, 3+mod);
            octave--;
            break;
        case 'd':
            out = A4_FREQ * pow(HALFTONE_STEP, 5+mod);
            octave--;
            break;
        case 'e':
            out = A4_FREQ * pow(HALFTONE_STEP, 7+mod);
            octave--;
            break;
        case 'f':
            out = A4_FREQ * pow(HALFTONE_STEP, 8+mod);
            octave--;
            break;
        case 'g':
            out = A4_FREQ * pow(HALFTONE_STEP, 10+mod);
            octave--;
            break;
    }

    if (octave>0) out *= 1<<octave;
    if (octave<0) out /= 1<<-octave;

    return out;
}
