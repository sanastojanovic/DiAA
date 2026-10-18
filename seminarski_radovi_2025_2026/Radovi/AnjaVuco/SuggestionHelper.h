#ifndef SUGGESTION_HELPER_H
#define SUGGESTION_HELPER_H

#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include "Word.h"

class SuggestionHelper {
private:
    static std::string toLowerCase(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return s;
    }

public:
    static int levenshteinDistance(const std::string& a, const std::string& b) {
        int n = (int)a.size();
        int m = (int)b.size();

        std::vector<std::vector<int>> dp(n + 1, std::vector<int>(m + 1, 0));

        for (int i = 0; i <= n; i++) dp[i][0] = i;
        for (int j = 0; j <= m; j++) dp[0][j] = j;

        for (int i = 1; i <= n; i++) {
            for (int j = 1; j <= m; j++) {
                int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;

                dp[i][j] = std::min({
                    dp[i - 1][j] + 1,
                    dp[i][j - 1] + 1,
                    dp[i - 1][j - 1] + cost
                });
            }
        }

        return dp[n][m];
    }

    static std::vector<std::string> rankAutocompleteSuggestions(
        const std::string& input,
        const std::vector<std::string>& candidates,
        const std::vector<Word>& words,
        int limit = 3
    ) {
        struct ScoredCandidate {
            std::string word;
            int wrongCount;
            int editDistance;
            int length;
            int score;
        };

        std::vector<ScoredCandidate> scored;
        std::string normalizedInput = toLowerCase(input);

        for (const auto& candidate : candidates) {
            int wrong = 0;

            for (const auto& w : words) {
                if (toLowerCase(w.english) == candidate) {
                    wrong = w.wrongCount;
                    break;
                }
            }

            int dist = levenshteinDistance(normalizedInput, candidate);
            int lenPenalty = (int)candidate.size() - (int)normalizedInput.size();
            if (lenPenalty < 0) lenPenalty = 0;

            int score = 5 * wrong - 2 * dist - lenPenalty;

            scored.push_back({
                candidate,
                wrong,
                dist,
                (int)candidate.size(),
                score
            });
        }

        std::sort(scored.begin(), scored.end(),
            [](const ScoredCandidate& a, const ScoredCandidate& b) {
                if (a.score != b.score)
                    return a.score > b.score;
                if (a.editDistance != b.editDistance)
                    return a.editDistance < b.editDistance;
                if (a.wrongCount != b.wrongCount)
                    return a.wrongCount > b.wrongCount;
                if (a.length != b.length)
                    return a.length < b.length;
                return a.word < b.word;
            });

        std::vector<std::string> result;
        for (int i = 0; i < (int)scored.size() && i < limit; i++) {
            result.push_back(scored[i].word);
        }

        return result;
    }

    static std::vector<std::string> getTopFuzzySuggestions(
        const std::string& input,
        const std::vector<Word>& words,
        int limit = 3,
        int maxDistance = 2
    ) {
        struct FuzzyCandidate {
            std::string word;
            int distance;
            int wrongCount;
            int score;
        };

        std::vector<FuzzyCandidate> candidates;
        std::string normalizedInput = toLowerCase(input);

        for (const auto& w : words) {
            std::string candidate = toLowerCase(w.english);
            int dist = levenshteinDistance(normalizedInput, candidate);

            if (dist <= maxDistance) {
                int score = 5 * w.wrongCount - 3 * dist;

                candidates.push_back({
                    w.english,
                    dist,
                    w.wrongCount,
                    score
                });
            }
        }

        std::sort(candidates.begin(), candidates.end(),
            [](const FuzzyCandidate& a, const FuzzyCandidate& b) {
                if (a.score != b.score)
                    return a.score > b.score;
                if (a.distance != b.distance)
                    return a.distance < b.distance;
                if (a.wrongCount != b.wrongCount)
                    return a.wrongCount > b.wrongCount;
                return a.word < b.word;
            });

        std::vector<std::string> result;
        for (int i = 0; i < (int)candidates.size() && i < limit; i++) {
            result.push_back(candidates[i].word);
        }

        return result;
    }
};

#endif