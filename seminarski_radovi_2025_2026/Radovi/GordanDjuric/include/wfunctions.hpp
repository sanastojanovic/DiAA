#ifndef __WFUNCTIONS_HPP
#define __WFUNCTIONS_HPP

#include <cmath>
#include <vector>

std::vector<float> noneWindow(int N) {
    std::vector<float> w(N,1.0f);
    return w;
}

std::vector<float> hannWindow(int N) {
    std::vector<float> w(N);
    for (int i = 0; i < N; i++)
    {
        w[i] = 0.5f * (1.0f - std::cos(2.0f * M_PI * i / (N - 1)));
    }
    return w;
}

std::vector<float> hammingWindow(int N) {
    std::vector<float> w(N);
    for (int i = 0; i < N; i++) {
        w[i] = 0.54f - 0.46f * std::cos(2.0f * M_PI * i / (N - 1));
    }
    return w;
}

std::vector<float> flatTopWindow(int N) {
    std::vector<float> w(N);
    for (int i = 0; i < N; i++) {
        float x = 2.0f * M_PI * i / (N - 1);
        w[i] = 0.21557895f
             - 0.41663158f * std::cos(x)
             + 0.277263158f * std::cos(2.0f * x)
             - 0.083578947f * std::cos(3.0f * x)
             + 0.006947368f * std::cos(4.0f * x);
    }
    return w;
}


std::vector<float> welchWindow(int N) {
    std::vector<float> w(N);
    float half = (N - 1) / 2.0f;
    for (int i = 0; i < N; i++) {
        float n = (i - half) / half;
        w[i] = 1.0f - n * n;
    }
    return w;
}


std::vector<float> parzenWindow(int N) {
    std::vector<float> w(N);
    float half = (N - 1) / 2.0f;

    for (int i = 0; i < N; i++) {
        float n = std::abs((i - half) / half);

        if (n <= 0.5f) {
            w[i] = 1.0f - 6.0f * n * n + 6.0f * n * n * n;
        } else if (n <= 1.0f) {
            float t = 1.0f - n;
            w[i] = 2.0f * t * t * t;
        } else {
            w[i] = 0.0f;
        }
    }
    return w;
}

#endif