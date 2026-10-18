#!/bin/bash

# Parametri
DICT="dictionary.txt"
TEST="test.txt"
K=3

# Provera da li ulazni fajlovi postoje
if [[ ! -f "$DICT" || ! -f "$TEST" ]]; then
    echo "Greska: Nedostaju $DICT ili $TEST. Pokreni prvo Python generator."
    exit 1
fi

echo "--- Kompajliranje ---"
# -O3 je obavezan na MATF-u za merenje performansi (omogucava agresivne optimizacije)
g++ -O3 main.cpp -o main_app
g++ -O3 naive.cpp -o naive_app

if [ $? -ne 0 ]; then
    echo "Greska pri kompajliranju!"
    exit 1
fi

echo "Sve je spremno. Zapocinjem testiranje..."
echo ""

# Testiranje tvog Trie resenja
echo "=============================="
echo "TEST: Optimizovani Trie (main)"
echo "=============================="
time ./main_app "$DICT" "$TEST" "$K"
echo ""

# Testiranje naivnog resenja
echo "=============================="
echo "TEST: Naivni pristup (naive)"
echo "=============================="
time ./naive_app "$DICT" "$TEST" "$K"
echo ""

echo "Poredjenje zavrseno. Proveri output.txt i output_naive.txt za korektnost."