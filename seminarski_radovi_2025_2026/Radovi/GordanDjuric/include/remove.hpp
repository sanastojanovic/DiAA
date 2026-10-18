#ifndef __REMOVE_HPP
#define __REMOVE_HPP

#include <vector>

#include "audio.hpp"
#include "utils.hpp"
#include "complex.hpp"
#include "wavheader.hpp"
#include "wfunctions.hpp"

#include "types.hpp"

std::vector<float> removeNoiseSTFT(const std::vector<float>& samples,const float procentage_removal, const WINDOW_FUNCTION wf = WINDOW_FUNCTION::HANN){
    int N = 1024;
    int hop = N/2;
    
    std::vector<float> window;
    if (wf == WINDOW_FUNCTION::NONE)
    {
        window = noneWindow(N);
    }else if (wf == WINDOW_FUNCTION::HAMMING)
    {
        window = hammingWindow(N);
    }else if (wf == WINDOW_FUNCTION::FLAT_TOP)
    {
        window = flatTopWindow(N);
    }else if (wf == WINDOW_FUNCTION::HANN)
    {
        window = hannWindow(N);
    }else if (wf == WINDOW_FUNCTION::PARZEN)
    {
        window = parzenWindow(N);
    }else if (wf == WINDOW_FUNCTION::WELCH)
    {
        window = welchWindow(N);
    }
    
    int numFrames = (samples.size() + hop - 1) / hop;
    std::vector<float> output(samples.size() + N, 0.0f); 
    std::vector<float> windowSums(output.size(), 0.0f);  

    ComplexVector noiseSpectrum(N);
    {
        ComplexVector firstFrame(N);
        for (int i = 0; i < N; i++) {
            float x = (i < samples.size()) ? samples[i] * window[i] : 0.0f;
            firstFrame[i] = Complex(x, 0.0f);
        }
        noiseSpectrum = fft(firstFrame);
        for (auto& c : noiseSpectrum) c = Complex(c.module(), 0.0f);
    }

     for (int f = 0; f < numFrames; f++) {
        ComplexVector frame(N);
        for (int i = 0; i < N; i++) {
            int idx = f * hop + i;
            float x = (idx < samples.size()) ? samples[idx] * window[i] : 0.0f;
            frame[i] = Complex(x, 0.0f);
        }

        // FFT
        ComplexVector spectrum = fft(frame);

        // Spectral subtraction (simple)
        for (int k = 0; k < N; k++) {
            float mag = spectrum[k].module() - procentage_removal*noiseSpectrum[k].real();
            if (mag < 0.0f) mag = 0.0f;
            float phase = std::atan2(spectrum[k].imag(), spectrum[k].real());
            spectrum[k] = Complex(mag * std::cos(phase), mag * std::sin(phase));
        }

        // IFFT
        ComplexVector cleanedFrame = ifft(spectrum);

        // Overlap-add
        for (int i = 0; i < N; i++) {
            int idx = f * hop + i;
            if (idx < output.size()) {
                output[idx] += cleanedFrame[i].real() * window[i];
                windowSums[idx] += window[i] * window[i];
            }
        }
    }

    for (size_t i = 0; i < samples.size(); i++) {
        if (windowSums[i] > 0.0f)
            output[i] /= windowSums[i];
    }

    output.resize(samples.size());
    return output;

}

#endif