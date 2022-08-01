#include <filesystem>

#include "wav.h"
#include "voices.h"

#include "config.h"

int main() {
    WavFile wavfile;
    Voice voice;

    for (auto& file : std::filesystem::directory_iterator(DATA_DIR)) {
        if (file.path().extension() == VOICE_SUFFIX) {
            voice.read_from_file((char*)file.path().string().c_str(), &wavfile.samples);
        }
    }

    wavfile.write((char*)DATA_DIR "/" OUTFILE);

    return 0;
}
