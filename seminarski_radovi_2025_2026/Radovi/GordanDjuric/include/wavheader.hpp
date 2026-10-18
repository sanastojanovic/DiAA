#ifndef __WAVHEADER_HPP
#define __WAVHEADER_HPP

#include <vector>
#include <cstdint>

#pragma pack(push, 1)

struct WAVHeader{
    char riff[4];
    uint32_t chunkSize;
    char wave[4];          
    char fmt[4];           
    uint32_t subchunk1Size;
    uint16_t audioFormat;  
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char data[4];
    uint32_t dataSize;
};

struct AudioBuffer{
    int sampleRate;
    int channels;
    std::vector<float> samples;
};

#pragma pack(pop)

#endif