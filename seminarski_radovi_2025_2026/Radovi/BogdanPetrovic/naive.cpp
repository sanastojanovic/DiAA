#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

// Struktura za čuvanje reči i frekvencije
typedef pair<int, string> Score;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Upotreba: " << argv[0] << " <recnik.txt> <test.txt> <K>" << endl;
        return 1;
    }

    string dictPath = argv[1];
    string testPath = argv[2];
    int K = stoi(argv[3]);

    // Naivna struktura: samo ravan niz svih parova
    vector<Score> dictionary;

    // 1. Učitavanje rečnika
    ifstream dict(dictPath);
    if (!dict) {
        cerr << "Greska pri otvaranju recnika!" << endl;
        return 1;
    }

    string line, word;
    int f;
    while (getline(dict, line)) {
        stringstream ss(line);
        if (ss >> word >> f) {
            dictionary.push_back({f, word});
        }
    }
    dict.close();

    // 2. Obrada upita
    ifstream test(testPath);
    ofstream out("output_naive.txt"); // Različito ime fajla da se ne bi prebrisao Trie output
    
    if (!test || !out) {
        cerr << "Greska pri otvaranju test ili output fajla!" << endl;
        return 1;
    }

    while (getline(test, word)) {
        if (word.empty()) continue;
        if (word.back() == '\r') word.pop_back(); // Fix za Windows line endings

        // NAIVNI ALGORITAM:
        // 1. Filtriraj sve koji počinju prefiksom
        vector<Score> matches;
        for (const auto& entry : dictionary) {
            // Provera da li entry.second počinje sa word
            if (entry.second.size() >= word.size() && 
                entry.second.compare(0, word.size(), word) == 0) {
                matches.push_back(entry);
            }
        }

        // 2. Sortiraj pronađene mečeve po frekvenciji (opadajuće)
        sort(matches.begin(), matches.end(), [](const Score& a, const Score& b) {
            if (a.first != b.first) return a.first > b.first;
            return a.second < b.second;
        });

        // 3. Upis prvih K u fajl
        out << word << ": ";
        for (int i = 0; i < min((int)matches.size(), K); ++i) {
            out << matches[i].second << (i == min((int)matches.size(), K) - 1 ? "" : ", ");
        }
        out << endl;
    }

    out.close();
    test.close();
    cout << "Naivna obrada zavrsena. Rezultati su u output_naive.txt" << endl;

    return 0;
}