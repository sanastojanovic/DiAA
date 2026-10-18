#ifndef PROGRESS_MANAGER_H
#define PROGRESS_MANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <algorithm>
#include "Word.h"

class ProgressManager {
public:
    static std::vector<Word> loadWordsFromBase(const std::string& filename) {
        std::vector<Word> words;
        std::ifstream inFile(filename);

        if (!inFile.is_open()) {
            std::cerr << "Error: Cannot open base file: " << filename << "\n";
            return words;
        }

        json j;
        inFile >> j;

        try {
            words = j.get<std::vector<Word>>();
        } catch (const std::exception& e) {
            std::cerr << "Error while parsing base file: " << e.what() << "\n";
        }

        return words;
    }

    static std::vector<Word> loadProgressOrBase(const std::string& baseFile,
                                                const std::string& progressFile) {
        if (std::filesystem::exists(progressFile)) {
            std::ifstream inFile(progressFile);

            if (!inFile.is_open()) {
                std::cerr << "Error: Cannot open progress file: " << progressFile << "\n";
                return {};
            }

            json j;
            inFile >> j;

            try {
                return j.get<std::vector<Word>>();
            } catch (const std::exception& e) {
                std::cerr << "Error while parsing progress file: " << e.what() << "\n";
                return {};
            }
        }

        return loadWordsFromBase(baseFile); //ako nema progress.json fajla, ucitaj words.json
    }

    static bool saveProgress(const std::string& filename, const std::vector<Word>& words) {
        std::ofstream outFile(filename);

        if (!outFile.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << "\n";
            return false;
        }

        json j = words;
        outFile << j.dump(4);

        return true;
    }

    static void printWords(const std::vector<Word>& words) {
        std::cout << "\n=== CURRENT WORD DATA ===\n";
        for (const auto& word : words) {
            std::cout << "Serbian: " << word.serbian
                      << " | English: " << word.english
                      << " | Correct: " << word.correctCount
                      << " | Wrong: " << word.wrongCount
                      << " | Level: " << word.masteryLevel
                      << " | Weight: " << word.learningWeight
                      << " | Interval(days): " << word.intervalDays
                      << "\n";
        }
    }

    static void printTopHardestWords(const std::vector<Word>& words, int limit = 5) {
        if (words.empty()) {
            std::cout << "No words available.\n";
            return;
        }

        std::vector<Word> sortedWords = words;

        std::sort(sortedWords.begin(), sortedWords.end(),
            [](const Word& a, const Word& b) {
                if (a.learningWeight != b.learningWeight)
                    return a.learningWeight > b.learningWeight;
                if (a.wrongCount != b.wrongCount)
                    return a.wrongCount > b.wrongCount;
                if (a.masteryLevel != b.masteryLevel)
                    return a.masteryLevel < b.masteryLevel;
                return a.serbian < b.serbian;
            });

        std::cout << "\n=== TOP " << limit << " HARDEST WORDS ===\n";

        int actualLimit = std::min(limit, (int)sortedWords.size());

        for (int i = 0; i < actualLimit; i++) {
            const Word& w = sortedWords[i];

            std::cout << (i + 1) << ". "
                      << w.serbian << " -> " << w.english
                      << " | Weight: " << w.learningWeight
                      << " | Wrong: " << w.wrongCount
                      << " | Level: " << w.masteryLevel
                      << " | Correct: " << w.correctCount
                      << "\n";
        }
    }
};

#endif