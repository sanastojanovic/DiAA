#ifndef __UTILS_HPP
#define __UTILS_HPP

#include <vector>
#include <climits>

#include "complex.hpp"

/**
 * Ovaj header file sadrzi sve potrebne funckije za funkcionisanje fft-a
 * kao i neka jednostavnija izracunavanja, potrebna za dio koji koristi masinsko ucenje
 */


typedef std::vector<Complex> ComplexVector; // omogucava jezgrovitiji zapis

/**
 * Pomocne funkcije
 */
 
 unsigned getNearestPower(int n, int base){
    unsigned tmp = 1;
    while(tmp <= n)
        tmp *= base;
    return tmp;
 }



/**
 * 1D FFT!
 */

void fft(const ComplexVector& a, int start_a,const ComplexVector& w, ComplexVector& rezultat, int start_rezultat, int korak){
    int n = a.size()/korak;
    if (n == 1)
    {
        rezultat[start_rezultat] = a[start_a];
        return ;
    }

    fft(a,start_a,w,rezultat,start_rezultat,korak*2);
    fft(a,start_a+korak,w,rezultat,start_rezultat+n/2,korak*2);
    
    for (int k = 0; k < n/2; k++)
    {
        Complex r1 = rezultat[start_rezultat+k];
        Complex r2 = rezultat[start_rezultat+(k+n/2)];

        rezultat[start_rezultat+k] = round_complex(r1 + w[k*korak]*r2);
        rezultat[start_rezultat+(k+n/2)] = round_complex(r1 - w[k*korak]*r2);
    }   
}

ComplexVector fft(const ComplexVector& a){
    int n = a.size();
    ComplexVector w(n/2);
    for (int i = 0; i < w.size(); i++)
        w[i] = Complex(1,2*i*M_PI/n,true);
    ComplexVector rezultat(n);
    fft(a,0,w,rezultat,0,1);
    return rezultat;
}

ComplexVector ifft(const ComplexVector& a){
    int n = a.size();
    ComplexVector w(n/2);
    for (int i = 0; i < w.size(); i++)
        w[i] = Complex(1,-2*i*M_PI/n,true);
    ComplexVector rezultat(n);
    fft(a,0,w,rezultat,0,1);
    for(int i = 0;i < rezultat.size();i++)
        rezultat[i] = round_complex(rezultat[i]*(1/((float)n)));
    return rezultat;
}



#endif