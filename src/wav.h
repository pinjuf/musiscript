#pragma once

#include <cstdint>
#include <vector>

#include "logging.h"
#include "config.h"

typedef struct StereoSample {
    int16_t l, r;
} StereoSample;

class WavFile {
    public:
        typedef struct wavheader_t {
            uint8_t RIFF[4]        = {'R', 'I', 'F', 'F'};
            uint32_t ChunkSize;
            uint8_t WAVE[4]        = {'W', 'A', 'V', 'E'};

            uint8_t FMT[4]         = {'f', 'm', 't', ' '};
            uint32_t Subchunk1Size = 16;
            uint16_t AudioFormat   = 1;      // Simple PCM format
            uint16_t NumChannels   = 2;      // 2 Channels (Stereo)
            uint32_t SampleRate    = SAMPLING_RATE;  // 44kHz sampling rate
            uint32_t ByteRate;               // SampleRate*BitsPerSample*NumChannels/8
            uint16_t BlockAlign;             // BitsPerSample*NumChannels/8 (16 bit Stereo);
            uint16_t BitsPerSample = 16;     // 16 Bits per sample

            uint8_t DATA[4] = {'d', 'a', 't', 'a'};
            uint32_t Subchunk2Size;
        } __attribute__((packed)) wavheader_t;
        wavheader_t wavheader;

        constexpr static double max_amp = INT16_MAX;
        constexpr static double def_amp = max_amp * 0.25; // the base amplitude, not too silent, but leaves enough room for some fun

        std::vector<StereoSample> samples;

        void fill_fmtvalues();
        void fill_sizevalues();
        void write(const char * filename);
};
