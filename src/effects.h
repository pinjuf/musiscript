#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>
#include <cstdint>

#include "wav.h"

enum EFFECTS {
    NO_EFFECT,
    CUTOFF,
    POW,
    AMP_VIBRATO,
    AD_ENVELOPE,
    CUSTOM0,
    CUSTOM1,
    CUSTOM2,
    CUSTOM3,
};

class Effect {
    public:
        EFFECTS effect;
        double settings[32] = {0};
        void get_through_effect(StereoSample * sample, uint64_t sample_count);
};

#endif
