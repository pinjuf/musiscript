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
    CUSTOM0,
    CUSTOM1,
    CUSTOM2,
    CUSTOM3,
};

int16_t get_sound_at_wavready(double i, double freq, SOUNDS sound, double amp = 1, double hz = SAMPLING_RATE);

#endif
