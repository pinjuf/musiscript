#pragma once

#include "wav.h"

enum SOUNDS {
    SILENCE,
    SINE,
    SQUARE,
    TRIANGLE,
    SAW,
    WHITE_NOISE,
    SND_CUSTOM0,
    SND_CUSTOM1,
    SND_CUSTOM2,
    SND_CUSTOM3,
};

constexpr char SOUNDNAMES[10][32] = {
    "NONE",
    "SINE",
    "SQUARE",
    "TRIANGLE",
    "SAW",
    "WHITE_NOISE",
    "CUSTOM0",
    "CUSTOM1",
    "CUSTOM2",
    "CUSTOM3",
};

int16_t get_sound_at_wavready(double i, double freq, SOUNDS sound, double amp = 1, double hz = SAMPLING_RATE);

SOUNDS get_sound_by_name(const char * name);
