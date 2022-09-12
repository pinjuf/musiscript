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

    std::filesystem::directory_iterator voice_files;
    try {
        voice_files = std::filesystem::directory_iterator(DATA_DIR);
    } catch (std::filesystem::filesystem_error & e) {
        log(LOG_FATAL, "Failed to open data directory");
        return 1;
    }

    for (auto& file : voice_files) {
        if (file.path().extension() == VOICE_SUFFIX) {
            voice = new Voice();
            log(LOG_INFO, "Preprossing voice file " + file.path().string());
            voice->preprocess_file((char *)file.path().string().c_str(), (char *)(file.path().string()+PREPROCESSED_VOICE_SUFFIX).c_str());
            log(LOG_INFO, ("Reading voice file " + file.path().string()+PREPROCESSED_VOICE_SUFFIX).c_str());
            voice->read_from_file((char*)(file.path().string()+PREPROCESSED_VOICE_SUFFIX).c_str(), &wavfile.samples);
            log(LOG_INFO, ("Done reading voice file " + file.path().string()+PREPROCESSED_VOICE_SUFFIX).c_str());
            delete voice;
        }
    }

    log(LOG_INFO, "Writing final .wav file");
    wavfile.write((char*)DATA_DIR "/" OUTFILE);
    log(LOG_INFO, "Final .wav file ready at " DATA_DIR "/" OUTFILE);

    return 0;
}
