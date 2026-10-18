#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <ctime>
#include <limits>
#include "Word.h"

class Scheduler {
private:
    static int calculatePriority(const Word& word) {
        std::time_t now = std::time(nullptr);

        int overdueBonus = 0;
        if (word.nextReviewTime <= now) {
            int overdueDays = static_cast<int>((now - word.nextReviewTime) / (24 * 60 * 60));
            if (overdueDays < 0) {
                overdueDays = 0;
            }
            overdueBonus = overdueDays;
        }

        int priority =
            10 * word.wrongCount +
            4 * word.learningWeight -
            5 * word.masteryLevel +
            overdueBonus;

        return priority;
    }

public:
    static bool hasReadyWords(const std::vector<Word>& words) {
        std::time_t now = std::time(nullptr);

        for (const auto& word : words) {
            if (word.nextReviewTime <= now) {
                return true;
            }
        }

        return false;
    }

    static int getNextWordIndex(const std::vector<Word>& words) {
        std::time_t now = std::time(nullptr);

        int bestIndex = -1;
        int bestPriority = std::numeric_limits<int>::min();

        for (int i = 0; i < (int)words.size(); i++) {
            const Word& word = words[i];

            if (word.nextReviewTime <= now) {
                int priority = calculatePriority(word);

                if (priority > bestPriority) {
                    bestPriority = priority;
                    bestIndex = i;
                }
            }
        }

        return bestIndex;
    }
};

#endif