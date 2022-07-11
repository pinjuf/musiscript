#include "effects.h"
#include "wav.h"

void Effect::get_through_effect(StereoSample * sample, uint64_t sample_count) {
    double l = sample->l/WavFile::def_amp;
    double r = sample->r/WavFile::def_amp;

    double timeinto = ((float) sample_count)/SAMPLING_RATE;

    switch (effect) {
        case CUTOFF:
            l = fmax(fmin(l, settings[0]), -settings[0]);
            r = fmax(fmin(r, settings[0]), -settings[0]);
            break;
        case POW:
            l = pow(l*settings[1], settings[0]);
            r = pow(r*settings[1], settings[0]);
            break;
        case AMP_VIBRATO:
            l *= sin(2*M_PI*sample_count*settings[0]/SAMPLING_RATE);
            r *= sin(2*M_PI*sample_count*settings[0]/SAMPLING_RATE);
            break;
        case AD_ENVELOPE: {
            double factor = 1;
            double attack = timeinto*1/settings[0];
            double decay = timeinto*-1/settings[1] + settings[0]*1/settings[1] + 1;

            factor = fmax(fmin(attack, decay), 0);

            l *= factor;
            r *= factor;
            
            break;
        }
        case NO_EFFECT:
        default:
            break;
    }

    sample->l = l*WavFile::def_amp;
    sample->r = r*WavFile::def_amp;
}
