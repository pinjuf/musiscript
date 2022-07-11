#include "notes.h"

double get_freq_by_name(char * name, int transpose) {
    char base = name[0];
    if (isupper(base)) base += 32;

    double out;
    int mod = 0;

    for (int i = 2; name[i]; i++) {
        switch (name[i]) {
            case '+':
            case '#':
                mod = 1;
                break;
            case '-':
            case 'b':
                mod = -1;
                break;
                break;
            default:
                mod = 0;
        }
        if (mod) break;
    }

    int octave = atoi(name+1) - 4;

    switch (base) {
        case 'a':
            out = A4_FREQ * pow(HALFTONE_STEP, 0+mod+transpose);
            break;
        case 'h':
        case 'b':
            out = A4_FREQ * pow(HALFTONE_STEP, 2+mod+transpose);
            break;
        case 'c':
            out = A4_FREQ * pow(HALFTONE_STEP, 3+mod+transpose);
            octave--;
            break;
        case 'd':
            out = A4_FREQ * pow(HALFTONE_STEP, 5+mod+transpose);
            octave--;
            break;
        case 'e':
            out = A4_FREQ * pow(HALFTONE_STEP, 7+mod+transpose);
            octave--;
            break;
        case 'f':
            out = A4_FREQ * pow(HALFTONE_STEP, 8+mod+transpose);
            octave--;
            break;
        case 'g':
            out = A4_FREQ * pow(HALFTONE_STEP, 10+mod+transpose);
            octave--;
            break;
    }

    if (octave>0) out *= 1<<octave;
    if (octave<0) out /= 1<<-octave;

    return out;
}
