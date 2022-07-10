#include "wav.h"
#include "sounds.h"
#include "notes.h"
#include "voices.h"

#include <iostream>

int main() {
    WavFile wavfile;
    Voice voice;

    voice.read_from_file((char*)"data/1.vc", &wavfile.samples);
    voice.read_from_file((char*)"datausc/2.vc", &wavfile.samples);

    wavfile.write((char*)"datausc/out.wav");

    return 0;
}
