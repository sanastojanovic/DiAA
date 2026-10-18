# Autocomplete Sistem: Weighted Trie sa Top-K Kesiranjem

Ovaj projekat predstavlja implementaciju i analizu efikasne strukture podataka za problem rangiranog automatskog dopunjavanja teksta (autocomplete). Projekat je razvijen u okviru seminarskog rada iz predmeta Dizajn i analiza algoritama (MATF).

## Sadrzaj projekta

### Izvorni kod (C++)
* main.cpp - Optimalno resenje. Implementacija Weighted Trie strukture sa Word ID sistemom i Top-K kesiranjem u cvorovima. Slozenost pretrage: O(L + K * W_avg).
* naive.cpp - Naivno resenje. Koristi std::vector i linearnu pretragu O(N) kroz ceo recnik pri svakom upitu. Sluzi kao benchmark za poredjenje performansi.

### Skripte i alati
* generate_tests.py - Python skripta za generisanje sintetickog recnika (dictionary.txt) i skupa upita (test.txt).
* benchmark.sh - Bash skripta koja automatizuje kompajliranje sa -O3 optimizacijom i meri vreme izvrsavanja oba resenja.

### Podaci
* dictionary.txt - Ulazni fajl sa recima i frekvencijama.
* test.txt - Ulazni fajl sa upitima.
* output.txt - Rezultati pretrage glavnog programa.
* output_naive.txt - Rezultati pretrage naivnog programa.

---

## Uputstvo za pokretanje

1. Generisanje podataka:
   python3 generate_tests.py

2. Automatsko testiranje i poredjenje performansi:
   chmod +x benchmark.sh
   ./benchmark.sh

3. Manuelno pokretanje (main.cpp):
   g++ -O3 main.cpp -o main_app
   ./main_app dictionary.txt test.txt 5

## Sistemski zahtevi
* C++14 ili noviji kompajler (GCC/Clang)
* Python 3.x
* Bash shell okruzenje (Linux, macOS ili WSL)