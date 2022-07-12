#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>
#include <cstdint>

#include "wav.h"

enum EFFECTS {
    NO_EFFECT,
    AMP_CUTOFF,
    AMP_POW,
    AMP_VIBRATO,
    AMP_AD_ENVELOPE,
    AMP_BITCRUSHER,
    EFF_CUSTOM0,
    EFF_CUSTOM1,
    EFF_CUSTOM2,
    EFF_CUSTOM3,
};

class Effect {
    public:
        EFFECTS effect;
        double settings[32] = {0};
        void get_through_amp_effect(StereoSample * sample, uint64_t sample_count);
        double get_through_freq_effect(double freq);
};

#endif
