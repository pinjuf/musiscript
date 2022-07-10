#ifndef VOICES_H
#define VOICES_H

#include <vector>
#include <string>
#include <sstream>

#include "wav.h"
#include "sounds.h"

class Voice {
    public:
        void voice_from_file(char * path);

        double stereo_pan;
        double volume;
        double speed;
        double sound;
};

#endif
