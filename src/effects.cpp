#include "effects.h"
#include "wav.h"

void Effect::get_through_amp_effect(StereoSample * sample, uint64_t sample_count) {
    double l = sample->l/WavFile::def_amp;
    double r = sample->r/WavFile::def_amp;

    double timeinto = ((double) sample_count)/SAMPLING_RATE;

    switch (effect) {
        case AMP_CUTOFF:
            l = fmax(fmin(l, settings[0]), -settings[0]);
            r = fmax(fmin(r, settings[0]), -settings[0]);
            break;
        case AMP_POW:
            l = pow(l*settings[1], settings[0]);
            r = pow(r*settings[1], settings[0]);
            break;
        case AMP_TREMOLO:
            l *= sin(2*M_PI*sample_count*settings[0]/SAMPLING_RATE);
            r *= sin(2*M_PI*sample_count*settings[0]/SAMPLING_RATE);
            break;
        case AMP_AD_ENVELOPE: {
            double factor = 1;
            double attack = timeinto*1/settings[0];
            double decay = timeinto*-1/settings[1] + settings[0]*1/settings[1] + 1;

            factor = fmax(fmin(attack, decay), 0);

            l *= factor;
            r *= factor;
            
            break;
        }
        case AMP_BITCRUSHER: {
            uint16_t l2 = (uint16_t) (l*WavFile::def_amp);
            uint16_t r2 = (uint16_t) (r*WavFile::def_amp);

            l2 >>= (int)settings[0]; // Divide, dropping least significant bits
            r2 >>= (int)settings[0];
            l2 <<= (int)settings[0]; // Multiply to original scale, but with dropped bits
            r2 <<= (int)settings[0];

            l = (double)l2/WavFile::def_amp;
            r = (double)r2/WavFile::def_amp;
        }
        case NO_EFFECT:
        default:
            break;
    }

    sample->l = l*WavFile::def_amp;
    sample->r = r*WavFile::def_amp;
}

double Effect::get_through_freq_effect(double freq, uint64_t sample_count) {
    switch (effect) {
        case NO_EFFECT:
        default:
            break;
    }

    return freq;
}

uint64_t Effect::get_through_i_effect(uint64_t i, uint64_t sample_count) {
    switch (effect) {
        case I_VIBRATO: {
            double timeinto = ((double) sample_count)/SAMPLING_RATE;
            i += sin(2*M_PI*timeinto*settings[0])*settings[1];
            break;
        }
        case I_WAH: {
            double timeinto = ((double) sample_count)/SAMPLING_RATE;
            i = (timeinto*settings[0]-log(timeinto*settings[0]+1))*SAMPLING_RATE/settings[0];
        }
        case NO_EFFECT:
        default:
            break;
    }

    return i;
}
