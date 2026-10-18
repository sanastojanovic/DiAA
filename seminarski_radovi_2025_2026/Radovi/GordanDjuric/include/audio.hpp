#ifndef __AUDIO_HPP
#define __AUDIO_HPP

#include <random>
#include <vector>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <algorithm>
#include <stdexcept>


#include "complex.hpp"
#include "wavheader.hpp"

#include "types.hpp"

AudioBuffer loadWAV(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("Cannot open WAV");

    char id[4];
    uint32_t size;

    file.read(id, 4);                 
    if (std::strncmp(id, "RIFF", 4))
        throw std::runtime_error("Not RIFF");

    file.read(reinterpret_cast<char*>(&size), 4); 

    file.read(id, 4);                 
    if (std::strncmp(id, "WAVE", 4))
        throw std::runtime_error("Not WAVE");

    uint16_t audioFormat = 0;
    uint16_t channels = 0;
    uint32_t sampleRate = 0;
    uint16_t bitsPerSample = 0;
    uint32_t dataSize = 0;

    
    while (file.read(id, 4)) {
        file.read(reinterpret_cast<char*>(&size), 4);

        if (!std::strncmp(id, "fmt ", 4)) {
            file.read(reinterpret_cast<char*>(&audioFormat), 2);
            file.read(reinterpret_cast<char*>(&channels), 2);
            file.read(reinterpret_cast<char*>(&sampleRate), 4);
            file.seekg(6, std::ios::cur); 
            file.read(reinterpret_cast<char*>(&bitsPerSample), 2);
            file.seekg(size - 16, std::ios::cur);
        }
        else if (!std::strncmp(id, "data", 4)) {
            dataSize = size;
            break;
        }
        else {
            file.seekg(size, std::ios::cur); 
        }
    }

    if (audioFormat != 1 || bitsPerSample != 16)
        throw std::runtime_error("Unsupported WAV");

    std::vector<int16_t> raw(dataSize / 2);
    file.read(reinterpret_cast<char*>(raw.data()), dataSize);

    AudioBuffer audio;
    audio.sampleRate = sampleRate;
    audio.channels = channels;
    audio.samples.resize(raw.size());

    for (size_t i = 0; i < raw.size(); i++)
        audio.samples[i] = raw[i] / 32768.0f;

    return audio;
}

std::vector<float> toMono(const AudioBuffer& buffer){
    if(buffer.channels == 1)
        return buffer.samples;
    std::vector<float> mono;
    mono.reserve(buffer.samples.size()/buffer.channels);
    for (size_t i = 0; i < buffer.samples.size(); i+=buffer.channels)
    {
        float sum = 0.0f;
        for (int ch = 0; ch < buffer.channels; ch++)
        {
            sum += buffer.samples[i+ch];
        }
        mono.push_back(sum/buffer.channels);
    }
    return mono;
}

void writeWAV(const std::string& filename, const std::vector<float>& samples, int sampleRate, int channels=1){
    std::ofstream file(filename,std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Cannot open output WAV file");
    }
    
    std::vector<int16_t> pcm(samples.size());
    for (size_t i = 0; i < samples.size(); i++) {
        double x = std::clamp(static_cast<double>(samples[i]), -1.0, 1.0);
        pcm[i] = static_cast<int16_t>(x * 32767.0);
    }

    WAVHeader header;

    std::memcpy(header.riff,"RIFF",4);
    std::memcpy(header.wave,"WAVE",4);
    std::memcpy(header.fmt,"fmt ",4);
    std::memcpy(header.data,"data",4);

    header.subchunk1Size = 16;
    header.audioFormat = 1;

    header.bitsPerSample = 16;

    header.numChannels = channels;
    header.sampleRate = sampleRate;
    header.blockAlign = channels * sizeof(int16_t);
    header.byteRate = sampleRate * header.blockAlign;
    header.dataSize = pcm.size() * sizeof(int16_t);
    header.chunkSize = 36 + header.dataSize;

    file.write(reinterpret_cast<char*>(&header), sizeof(WAVHeader));
    file.write(reinterpret_cast<char*>(pcm.data()), header.dataSize);
}

std::vector<float> addNoise(const std::vector<float>& samples,
                            const float stddev,
                            const NOISE_TYPE noise = NOISE_TYPE::NORMAL)
{
    std::random_device rd;
    std::mt19937 gen(rd());

    std::normal_distribution<float> normalDist(0.0f, stddev);
    std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);

    // State for pink noise (simple Voss-style filter)
    float pinkState = 0.0f;

    std::vector<float> noisy;
    noisy.reserve(samples.size());

    for (auto s : samples) {
        float noiseSample = 0.0f;

        if (noise == NOISE_TYPE::NORMAL)
        {
            noiseSample = normalDist(gen);
        } else if (noise == NOISE_TYPE::PINK)
        {
            float white = normalDist(gen);
            pinkState = 0.98f * pinkState + 0.02f * white;
            noiseSample = pinkState;
        } else if (noise == NOISE_TYPE::IMPULSIVE)
        {
            if (uniformDist(gen) < 0.01f) { 
                noiseSample = normalDist(gen) * 5.0f;
            } else {
                noiseSample = 0.0f;
            }
        }
        
        float n = s + noiseSample;
        if (n > 1.0f) n = 1.0f;
        if (n < -1.0f) n = -1.0f;

        noisy.push_back(n);
    }

    return noisy;
}

#endif