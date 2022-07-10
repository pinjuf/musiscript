#include "wav.h"

void WavFile::fill_fmtvalues() {
    wavheader.ByteRate = wavheader.SampleRate * wavheader.BitsPerSample * wavheader.NumChannels / 8;
    wavheader.BlockAlign = wavheader.BitsPerSample * wavheader.NumChannels / 8;
}

void WavFile::fill_sizevalues() {
    wavheader.Subchunk2Size = 4 + samples.size() * wavheader.BlockAlign;
    wavheader.ChunkSize = 36 + wavheader.Subchunk2Size;
}

void WavFile::write(char * filename) {
    std::ofstream outfile(filename, std::ios::binary);

    fill_fmtvalues();
    fill_sizevalues();

    outfile.write((char*)&wavheader, sizeof(wavheader));
    
    for (StereoSample sample : samples) {
        outfile.write((char*)&sample.l, sizeof(sample.l));
        outfile.write((char*)&sample.r, sizeof(sample.r));
    }

    outfile.close();
}
