#include <filesystem>

#include "wav.h"
#include "voices.h"

int main() {
    WavFile wavfile;
    Voice voice;

    for (auto& file : std::filesystem::directory_iterator("data")) {
        if (file.path().extension() == ".vc") {
            voice.read_from_file((char*)file.path().string().c_str(), &wavfile.samples);
        }
    }

    wavfile.write((char*)"data/out.wav");

    return 0;
}
