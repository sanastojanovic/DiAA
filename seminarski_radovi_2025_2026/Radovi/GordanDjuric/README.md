# Redukovanje šuma koristeći algoritam Brze Furijeove transformacije (FFT)

## Pregled

Ovaj seminarski ima za cilj da prikaže primjenu algoritma brze furijeove transformacije u polju obrade zvuka, 
tačnije redukovanja šuma iz samog zvuka. Implementacija čitave protočne obrade je odrađena u C++ i sadrži:

- Potpunu implementaciju artimetike kompleksnih brojeva
- Algoritam brze Furijeove transformacije
- Kratkoročna Furijeova transformacija:
    - Hann prozor
    - Hamming prozor
    - Welch prozor
    - Parzen prozor
- Spektralno odstranjenje šuma 
- Čitač i pisač WAV (16-bit) fajlova 
- Konverzija u mono audio
- Dodavanje šuma (korišteno za testiranje)

## Struktura projekta

```
├── DAA___seminarski.pdf            # opis samog seminarskog
├── data
│   ├── cleaned
│   │   ├── impulsive
│   │   ├── normal
│   │   └── pink
│   ├── rusija_mono.wav
│   ├── sample
│   │   ├── impulsive
│   │   ├── normal
│   │   └── pink
│   └── src
│       └── rusija.wav
├── include
│   ├── audio.hpp                   # header za rad sa WAV fajlovima
│   ├── complex.hpp                 # kompleksna aritmetika
│   ├── remove.hpp                  # algoritam STFT
│   ├── types.hpp                   # enum tipovi
│   ├── utils.hpp                   # implementacija FFT-a
│   ├── wavheader.hpp               # pomocne strukture za cuvanje WAV informacija
│   └── wfunctions.hpp              # prozorne funkcije
├── jupyter_spectrograms
│   └── spectral_noise.ipynb        # spektrogramska analiza svih audio fajlova
├── main.cpp                        # tlo za testiranje :)
├── Makefile                        
├── README.md
└── src
    └── complex.cpp

```

## Osobine audio fajlova za obradu

Da bi protočna obrada audio fajlova radila, potrebno je da sam audio fajl zadovolji par karakteristika.
Te karakteristike su:
- WAV format
- PCM kodiranje
- 16-bitno uzorkovanje

Ako je audio u nekom drugom formatu, moguće je izvršiti konverziju u terminalu na idući način:

```bash
ffmpeg -i input.mp3 -ac 1 -ar 44100 -c:a pcm_s16le output.wav
```
