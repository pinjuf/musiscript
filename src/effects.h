#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>
#include <cstdint>

#include "wav.h"

enum EFFECTS {
    NO_EFFECT,
    CUTOFF,
    POW,
    VIBRATO,
};

class Effect {
    public:
        EFFECTS effect;
        double settings[8];
};

void get_through_effect(StereoSample * sample, Effect effect, uint64_t sample_count);

#endif
