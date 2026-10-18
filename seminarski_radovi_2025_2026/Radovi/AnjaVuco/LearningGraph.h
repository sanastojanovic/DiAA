#ifndef LEARNING_GRAPH_H
#define LEARNING_GRAPH_H

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class LearningGraph {
private:
    std::vector<std::string> levelNames = {
        "New",
        "Learning",
        "Familiar",
        "Mastered"
    };

public:
    std::string getLevelName(int level) const {
        if (level < 0 || level >= (int)levelNames.size()) {
            return "Unknown";
        }
        return levelNames[level];
    }

    int advanceOnCorrect(int currentLevel) const {
        if (currentLevel < 3) {
            return currentLevel + 1;
        }
        return currentLevel;
    }

    int degradeOnWrong(int currentLevel) const {
        if (currentLevel > 0) {
            return currentLevel - 1;
        }
        return currentLevel;
    }

    int updateWeightOnCorrect(int currentWeight) const {
        return std::max(1, currentWeight - 1);
    }

    int updateWeightOnWrong(int currentWeight) const {
        return std::min(10, currentWeight + 2);
    }

    void printGraph() const {
        std::cout << "=== LEARNING GRAPH ===\n";
        std::cout << "Correct answer transitions:\n";
        std::cout << "  New -> Learning\n";
        std::cout << "  Learning -> Familiar\n";
        std::cout << "  Familiar -> Mastered\n";
        std::cout << "  Mastered -> Mastered\n\n";

        std::cout << "Wrong answer transitions:\n";
        std::cout << "  Mastered -> Familiar\n";
        std::cout << "  Familiar -> Learning\n";
        std::cout << "  Learning -> New\n";
        std::cout << "  New -> New\n\n";

        std::cout << "Weight update rules:\n";
        std::cout << "  Correct answer: weight - 1 (minimum 1)\n";
        std::cout << "  Wrong answer: weight + 2 (maximum 10)\n";
    }
};

#endif