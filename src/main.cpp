#include <filesystem>
#include <random>
#include <time.h>

#include "wav.h"
#include "voices.h"
#include "config.h"
#include "logging.h"

int main() {
    WavFile wavfile;
    Voice * voice;

#if CONSTANT_SRAND
    srand(0);
#else
    srand(time(0));
#endif

    for (auto& file : std::filesystem::directory_iterator(DATA_DIR)) {
        if (file.path().extension() == VOICE_SUFFIX) {
            voice = new Voice();
            log(LOG_INFO, ("Reading voice file " + file.path().string()).c_str());
            voice->read_from_file((char*)file.path().string().c_str(), &wavfile.samples);
            log(LOG_INFO, ("Done reading voice file " + file.path().string()).c_str());
            delete voice;
        }
    }

    log(LOG_INFO, "Writing final .wav file");
    wavfile.write((char*)DATA_DIR "/" OUTFILE);
    log(LOG_INFO, "Final .wav file ready at " DATA_DIR "/" OUTFILE);

    return 0;
}
