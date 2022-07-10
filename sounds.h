#ifndef SOUND_H
#define SOUND_H

#include <cstdint>
#include <cmath>

#include "wav.h"

enum SOUNDS {
    SILENCE,
    SINE,
    SQUARE,
    TRIANGLE,
    SAW,
};

int16_t get_sound_at_wavready(double i, double freq, SOUNDS sound, double amp = 0.25, double hz = 44100);

#endif
