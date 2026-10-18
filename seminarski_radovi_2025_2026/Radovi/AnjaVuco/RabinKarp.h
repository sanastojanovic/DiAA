#ifndef RABIN_KARP_H
#define RABIN_KARP_H

#include <string>

class RabinKarp {
private:
    static const int d = 256;   // broj karaktera
    static const int q = 101;   // prost broj

public:
    static bool search(const std::string& text, const std::string& pattern) {
        int n = text.size();
        int m = pattern.size();

        if (m > n) return false;

        int p = 0; // hash pattern
        int t = 0; // hash text
        int h = 1;

        // h = pow(d, m-1) % q
        for (int i = 0; i < m - 1; i++)
            h = (h * d) % q;

        // inicijalni hash
        for (int i = 0; i < m; i++) {
            p = (d * p + pattern[i]) % q;
            t = (d * t + text[i]) % q;
        }

        for (int i = 0; i <= n - m; i++) {

            if (p == t) {
                // proveri karakter po karakter
                bool match = true;
                for (int j = 0; j < m; j++) {
                    if (text[i + j] != pattern[j]) {
                        match = false;
                        break;
                    }
                }
                if (match) return true;
            }

            // pomeri prozor
            if (i < n - m) {
                t = (d * (t - text[i] * h) + text[i + m]) % q;
                if (t < 0) t += q;
            }
        }

        return false;
    }
};

#endif