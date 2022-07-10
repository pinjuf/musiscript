#include "wav.h"
#include "sounds.h"
#include "notes.h"
#include "voices.h"

#include <iostream>

int main() {
    /*
    WavFile wavfile;
    StereoSample sample;


    for (int j = 0; j < 8; j++) {
    for (int i = 0; i < 44100>>2; i++) {
        sample.l = get_sound_at_wavready(i, get_freq_by_name((char*)"C4"), SINE);
        sample.r = sample.l;
        wavfile.samples.push_back(sample);
    }
    for (int i = 0; i < 44100>>2; i++) {
        sample.l = get_sound_at_wavready(i, get_freq_by_name((char*)"E4-"), SQUARE);
        sample.r = sample.l;
        wavfile.samples.push_back(sample);
    }
    for (int i = 0; i < 44100>>2; i++) {
        sample.l = get_sound_at_wavready(i, get_freq_by_name((char*)"G4"), TRIANGLE);
        sample.r = sample.l;
        wavfile.samples.push_back(sample);
    }
    for (int i = 0; i < 44100>>2; i++) {
        sample.l = get_sound_at_wavready(i, get_freq_by_name((char*)"E4-"), SAW);
        sample.r = sample.l;
        wavfile.samples.push_back(sample);
    }
    }

    wavfile.write((char*)"project.musc/out.wav");
    */

    WavFile wavfile;
    Voice voice;

    voice.read_from_file((char*)"project.musc/1.vc", &wavfile.samples);

    wavfile.write((char*)"project.musc/out.wav");

    return 0;
}
