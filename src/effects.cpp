#include <cmath>
#include <cstdint>
#include <cstring>

#include "effects.h"
#include "wav.h"

void Effect::get_through_amp_effect(StereoSample * sample, size_t rel_sample_count, size_t abs_sample_count) {
    // Explanation of the two sample counts:
    // Since musiscript allows for the dynamic adding and removal of effects, they must know when they are active
    // Adding and removing them from a stack works for most effects, except those that need to reference other samples. (BUF_EFFECTS)
    // Therefore, each effect has a start and end sample count, which it checks against the absolute sample count to see if it should actually apply.
    // But nevertheless, most effects still need to know the relative sample count (relative to the beginning of the note).
    // This leads to having to pass both sample counts to the effect.

    if (start >= abs_sample_count || abs_sample_count >= end) {
        return;
    }

    double l = sample->l/WavFile::def_amp;
    double r = sample->r/WavFile::def_amp;

    double timeinto = ((double) rel_sample_count)/SAMPLING_RATE;

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
            l *= sin(2*M_PI*rel_sample_count*settings[0]/SAMPLING_RATE);
            r *= sin(2*M_PI*rel_sample_count*settings[0]/SAMPLING_RATE);
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
        case AMP_A_ENVELOPE: {
            double factor = fmin(1/settings[0]*rel_sample_count/SAMPLING_RATE, 1);
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

double Effect::get_through_freq_effect(double freq, size_t rel_sample_count, size_t abs_sample_count) {
    if (start >= abs_sample_count || abs_sample_count >= end) {
        return freq;
    }

    switch (effect) {
        case NO_EFFECT:
        default:
            break;
    }

    return freq;
}

size_t Effect::get_through_i_effect(size_t i, size_t rel_sample_count, size_t abs_sample_count) {
    if (start >= abs_sample_count || abs_sample_count >= end) {
        return i;
    }

    switch (effect) {
        case I_VIBRATO: {
            double timeinto = ((double) rel_sample_count)/SAMPLING_RATE;
            i += sin(2*M_PI*timeinto*settings[0])*settings[1];
            break;
        }
        case I_WAH: {
            double timeinto = ((double) rel_sample_count)/SAMPLING_RATE;
            i = (timeinto*settings[0]-log(timeinto*settings[0]+1))*SAMPLING_RATE/settings[0];
        }
        case NO_EFFECT:
        default:
            break;
    }

    return i;
}

void Effect::get_through_buffer_effect(std::vector<StereoSample> * buffer) {
    // Here, the 'end'-check must be performed individually for each effect, as they all go through the whole buffer
    switch (effect) {
        case BUF_SMOOTH: {
            std::vector<StereoSample> * tempbuffer = new std::vector<StereoSample>();
            for (size_t i = 0; i < buffer->size(); i++) {
                tempbuffer->push_back(buffer->at(i));
            }

            for (size_t i = 0; i < buffer->size(); i++) {
                if (start >= i || i >= end) {
                    continue;
                }
                int64_t l = 0;
                int64_t r = 0;
                for (int j = -settings[0]; j <= settings[0]; j++) {
                    if (i+j < buffer->size() && i+j >= 0) {
                        l += tempbuffer->at(i+j).l;
                        r += tempbuffer->at(i+j).r;
                    }
                }
                buffer->at(i).l = l/(settings[0]*2+1);
                buffer->at(i).r = r/(settings[0]*2+1);
            }
            break;
        }
        case BUF_ECHO: {
            size_t delay = settings[0]*SAMPLING_RATE;
            for (size_t i = delay; i < buffer->size(); i++) {
                if (start >= i || i >= end) {
                    continue;
                }
                buffer->at(i).l += buffer->at(i-delay).l*settings[1];
                buffer->at(i).r += buffer->at(i-delay).r*settings[1];
            }
            break;
        }
        case NO_EFFECT:
        default:
            break;
    }
}

EFFECTS Effect::get_effect_by_name(const char * name) {
    for (size_t i = 0; i < sizeof(EFFECTNAMES)/sizeof(EFFECTNAMES[0]); i++) {
        if (strcmp(name, EFFECTNAMES[i]) == 0) {
            return (EFFECTS) i;
        }
    }
    return NO_EFFECT;
}
