#include <iostream>

#include "utils.hpp"
#include "audio.hpp"
#include "complex.hpp"

#include "types.hpp"
#include "remove.hpp"

void clean_all(std::vector<float>& normalNoise,const std::vector<float>& pinkNoise,const std::vector<float>& impulsiveNoise, 
                const WINDOW_FUNCTION wf, const float thresh_procentage[5], 
                const unsigned sampleRate, const unsigned channels = 1){
    
    std::string applied_function;
    switch (wf)
    {
    case WINDOW_FUNCTION::NONE:
        std::cout << "Applying no window function" << std::endl;
        applied_function = "_none_";
        break;
    case WINDOW_FUNCTION::FLAT_TOP:
        std::cout << "Applying FLAT_TOP function" << std::endl;
        applied_function = "_flat_";
        break;
    case WINDOW_FUNCTION::HAMMING:
        std::cout << "Applying the Hamming window function" << std::endl;
        applied_function = "_hamming_";
        break;
    case WINDOW_FUNCTION::HANN:
        std::cout << "Applying the Hann window function" << std::endl;
        applied_function = "_hann_";
        break;
    case WINDOW_FUNCTION::PARZEN:
        std::cout << "Applying the Parzen function" << std::endl;
        applied_function = "_parzen_";
        break;
    case WINDOW_FUNCTION::WELCH:
        std::cout << "Applying the Welch function" << std::endl;
        applied_function = "_welch_";
        break;
    default:
        break;
    }
    
    
    for (int i = 0; i < 5; i++)
    {
        std::string thresh_str = std::to_string(thresh_procentage[i]);
        std::string message = "denoising... (factor: " + thresh_str + ")";

        std::cout << "Normal " << message << std::endl;
        std::vector<float> denoiseNormal = removeNoiseSTFT(normalNoise,thresh_procentage[i],wf);
        writeWAV("data/cleaned/normal/rusija" + applied_function + thresh_str + ".wav",denoiseNormal,sampleRate,channels);

        std::cout << "Pink " << message << std::endl;
        std::vector<float> denoisePink = removeNoiseSTFT(pinkNoise,thresh_procentage[i],wf);
        writeWAV("data/cleaned/pink/rusija" + applied_function + thresh_str + ".wav",denoisePink,sampleRate,channels);

        std::cout << "Impulsive " << message << std::endl;
        std::vector<float> denoiseImpulsive = removeNoiseSTFT(impulsiveNoise,thresh_procentage[i],wf);
        writeWAV("data/cleaned/impulsive/rusija" + applied_function + thresh_str + ".wav",denoiseImpulsive,sampleRate,channels);
    }
    
} 

int main() {
    auto a = loadWAV("data/src/rusija.wav");
    std::cout << "Loaded in the audio..." << std::endl;
    std::cout << "Converting to MONO audio" << std::endl;
    std::vector<float> monoAudio = toMono(a);
    writeWAV("data/rusija_mono.wav",monoAudio,a.sampleRate,1);
    std::cout << "Adding normal(white) noise..." << std::endl;
    std::vector<float> normal_noise = addNoise(monoAudio,0.2,NOISE_TYPE::NORMAL);
    writeWAV("data/sample/normal/rusija.wav", normal_noise, a.sampleRate, 1);
    std::cout << "Adding pink noise..." << std::endl;
    std::vector<float> pink_noise = addNoise(monoAudio,0.2f,NOISE_TYPE::PINK);
    writeWAV("data/sample/pink/rusija.wav", pink_noise, a.sampleRate, 1);
    std::cout << "Adding impulsive noise..." << std::endl;
    std::vector<float> imp_noise = addNoise(monoAudio,0.2f, NOISE_TYPE::IMPULSIVE);
    writeWAV("data/sample/impulsive/rusija.wav", imp_noise, a.sampleRate, 1);

    float thresh_procentages[5] = {0.2f, 0.5f, 1.0f, 1.5f, 2.0f};

    clean_all(normal_noise,pink_noise,imp_noise,WINDOW_FUNCTION::FLAT_TOP,thresh_procentages,a.sampleRate);
    clean_all(normal_noise,pink_noise,imp_noise,WINDOW_FUNCTION::HAMMING,thresh_procentages,a.sampleRate);
    clean_all(normal_noise,pink_noise,imp_noise,WINDOW_FUNCTION::HANN,thresh_procentages,a.sampleRate);
    clean_all(normal_noise,pink_noise,imp_noise,WINDOW_FUNCTION::NONE,thresh_procentages,a.sampleRate);
    clean_all(normal_noise,pink_noise,imp_noise,WINDOW_FUNCTION::WELCH,thresh_procentages,a.sampleRate);
    clean_all(normal_noise,pink_noise,imp_noise,WINDOW_FUNCTION::PARZEN,thresh_procentages,a.sampleRate);

    return 0;
}
