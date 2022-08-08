#ifndef EFFECTS_H
#define EFFECTS_H

#include <cmath>
#include <cstdint>

#include "wav.h"

enum EFFECTS {
    NO_EFFECT,
    AMP_CUTOFF,
    AMP_POW,
    AMP_TREMOLO,
    AMP_AD_ENVELOPE,
    AMP_A_ENVELOPE,
    AMP_BITCRUSHER,
    I_VIBRATO,
    I_WAH,
    BUF_SMOOTH,
    BUF_ECHO,
    EFF_CUSTOM0,
    EFF_CUSTOM1,
    EFF_CUSTOM2,
    EFF_CUSTOM3,
};

constexpr char EFFECTNAMES[32][15] {
    "NONE",
    "CUTOFF",
    "POW",
    "TREMOLO",
    "AD_ENVELOPE",
    "A_ENVELOPE",
    "BITCRUSHER",
    "VIBRATO",
    "WAH",
    "SMOOTH",
    "ECHO",
    "CUSTOM0",
    "CUSTOM1",
    "CUSTOM2",
    "CUSTOM3",
};

class Effect {
    public:
        EFFECTS effect = NO_EFFECT;
        double settings[32] = {0};
        void get_through_amp_effect(StereoSample * sample, uint64_t sample_count);
        double get_through_freq_effect(double freq, uint64_t sample_count);
        uint64_t get_through_i_effect(uint64_t i, uint64_t sample_count);
        void get_through_buffer_effect(std::vector<StereoSample> * buffer);
};

#endif
