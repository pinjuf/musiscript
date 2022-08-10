#ifndef EFFECTS_H
#define EFFECTS_H

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

constexpr char EFFECTNAMES[15][32] {
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
        void get_through_amp_effect(StereoSample * sample, size_t rel_sample_count, size_t abs_sample_count);
        double get_through_freq_effect(double freq, size_t rel_sample_count, size_t abs_sample_count);
        size_t get_through_i_effect(size_t i, size_t rel_sample_count, size_t abs_sample_count);
        void get_through_buffer_effect(std::vector<StereoSample> * buffer);
        size_t start = 0, end = SIZE_MAX;
        static EFFECTS get_effect_by_name(const char * name);
};

#endif
