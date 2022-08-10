#include <filesystem>

#include "wav.h"
#include "voices.h"
#include "config.h"
#include "logging.h"

int main() {
    WavFile wavfile;
    Voice voice;

    for (auto& file : std::filesystem::directory_iterator(DATA_DIR)) {
        if (file.path().extension() == VOICE_SUFFIX) {
            log(LOG_INFO, ("Reading voice file " + file.path().string()).c_str());
            voice.read_from_file((char*)file.path().string().c_str(), &wavfile.samples);
            log(LOG_INFO, ("Done reading voice file " + file.path().string()).c_str());
        }
    }

    log(LOG_INFO, "Writing final .wav file");
    wavfile.write((char*)DATA_DIR "/" OUTFILE);
    log(LOG_INFO, "Final .wav file ready at " DATA_DIR "/" OUTFILE);

    return 0;
}
