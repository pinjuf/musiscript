#include "sounds.h"

int16_t get_sound_at_wavready(double i, double freq, SOUNDS sound, double amp, double hz) {
    i /= hz;
    i *= freq;
    double out;

    switch (sound) {
        case SINE:
            out = sin(M_PI*2*i);
            break;
        case SQUARE:
            out = 4*floor(i)-2*floor(2*i)+1;
            break;
        case TRIANGLE:
            out = 4*std::abs(i-floor(i+3.0f/4)+1.0f/4)-1;
            break;
        case SAW:
            out = 2*(i-floor(i+1.0f/2));
            break;
        case WHITE_NOISE:
            out = (rand() % 32768) / 32768.0f;
            break;

        case SND_CUSTOM0:
            out = sin(M_PI*2*i)*exp(fmod(i/25, 4));
            break;
        case SND_CUSTOM1:
            break;
        case SND_CUSTOM2:
            break;
        case SND_CUSTOM3:
            break;

        case SILENCE:
        default:
            out = 0;
    }

    return fmax(fmin(out * amp * WavFile::def_amp, WavFile::max_amp), -WavFile::max_amp+1); // Clip audio at max volume
}
