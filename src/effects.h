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
    EFF_CUSTOM0,
    EFF_CUSTOM1,
    EFF_CUSTOM2,
    EFF_CUSTOM3,
};

class Effect {
    public:
        EFFECTS effect;
        double settings[32] = {0};
        void get_through_effect(StereoSample * sample, uint64_t sample_count);
};

#endif
