#include "effects.h"

void get_through_effect(StereoSample * sample, Effect effect, uint64_t sample_count) {
    double l = sample->l/WavFile::def_amp;
    double r = sample->r/WavFile::def_amp;

    switch (effect.effect) {
        case CUTOFF:
            l = fmax(fmin(l, effect.settings[0]), -effect.settings[0]);
            r = fmax(fmin(r, effect.settings[0]), -effect.settings[0]);
            break;
        case POW:
            l = pow(l*effect.settings[1], effect.settings[0]);
            r = pow(r*effect.settings[1], effect.settings[0]);
            break;
        case VIBRATO:
            l *= sin(2*M_PI*sample_count*effect.settings[0]/SAMPLING_RATE);
            r *= sin(2*M_PI*sample_count*effect.settings[0]/SAMPLING_RATE);
            break;
        case NO_EFFECT:
        default:
            break;
    }
}
