#ifndef WORD_H
#define WORD_H

#include <string>
#include <ctime>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Word {
    std::string serbian;
    std::string english;

    int correctCount = 0;
    int wrongCount = 0;
    int masteryLevel = 0;
    int intervalDays = 1;
    int learningWeight = 5;   // nova tezina reci za personalizovano ucenje
    std::time_t nextReviewTime = 0;

    Word() = default;

    Word(const std::string& sr, const std::string& en)
        : serbian(sr), english(en) {
        nextReviewTime = std::time(nullptr);
    }
};

inline void to_json(json& j, const Word& w) {
    j = json{
        {"serbian", w.serbian},
        {"english", w.english},
        {"correctCount", w.correctCount},
        {"wrongCount", w.wrongCount},
        {"masteryLevel", w.masteryLevel},
        {"intervalDays", w.intervalDays},
        {"learningWeight", w.learningWeight},
        {"nextReviewTime", w.nextReviewTime}
    };
}

inline void from_json(const json& j, Word& w) {
    j.at("serbian").get_to(w.serbian);
    j.at("english").get_to(w.english);

    w.correctCount = j.value("correctCount", 0);
    w.wrongCount = j.value("wrongCount", 0);
    w.masteryLevel = j.value("masteryLevel", 0);
    w.intervalDays = j.value("intervalDays", 1);
    w.learningWeight = j.value("learningWeight", 5);
    w.nextReviewTime = j.value("nextReviewTime", std::time(nullptr));
}

#endif