#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>

#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

#include "Word.h"
#include "ProgressManager.h"
#include "Trie.h"
#include "Scheduler.h"
#include "LearningGraph.h"
#include "SuggestionHelper.h"
#include "RabinKarp.h"

std::vector<Word> words;
Trie trie;
LearningGraph graph;

int currentIndex = -1;

// GUI
Fl_Box* wordBox;
Fl_Input* inputBox;
Fl_Box* resultBox;
Fl_Box* suggestionBox;
Fl_Box* statsBox;
Fl_Box* progressBox;
Fl_Box* hardestBox;
Fl_Box* hintBox;
Fl_Button* nextButton;
Fl_Button* resetButton;
Fl_Button* hardestButton;

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string getProgressBar(int masteryLevel) {
    int filled = 0;

    if (masteryLevel == 0) filled = 0;
    else if (masteryLevel == 1) filled = 2;
    else if (masteryLevel == 2) filled = 4;
    else if (masteryLevel >= 3) filled = 6;

    std::string bar = "[";
    for (int i = 0; i < 6; i++) {
        if (i < filled) bar += "#";
        else bar += "-";
    }
    bar += "]";

    return bar;
}

void rebuildTrie() {
    trie.clear();
    for (auto& w : words) {
        trie.insert(toLower(w.english));
    }
}

void updateStatsBox() {
    if (currentIndex == -1) {
        statsBox->copy_label("");
        progressBox->copy_label("");
        return;
    }

    const Word& w = words[currentIndex];
    std::string progressBar = getProgressBar(w.masteryLevel);

    std::string stats =
        "Level: " + graph.getLevelName(w.masteryLevel) +
        "   |   Weight: " + std::to_string(w.learningWeight) +
        "   |   Correct: " + std::to_string(w.correctCount) +
        "   |   Wrong: " + std::to_string(w.wrongCount);

    std::string progressText = "Progress: " + progressBar;

    statsBox->copy_label(stats.c_str());
    progressBox->copy_label(progressText.c_str());
}

void loadNextWord() {
    currentIndex = Scheduler::getNextWordIndex(words);

    if (currentIndex == -1) {
        if (!words.empty()) {
            currentIndex = 0;
            resultBox->copy_label("No scheduled words ready - showing first word for testing.");
        } else {
            wordBox->copy_label("No words available!");
            inputBox->value("");
            resultBox->copy_label("");
            suggestionBox->copy_label("");
            statsBox->copy_label("");
            progressBox->copy_label("");
            hintBox->copy_label("");
            return;
        }
    } else {
        resultBox->copy_label("");
    }

    std::string text = "Translate: " + words[currentIndex].serbian;
    wordBox->copy_label(text.c_str());

    inputBox->value("");
    suggestionBox->copy_label("");
    hintBox->copy_label("You may enter a single word, a phrase, or a full sentence.");

    updateStatsBox();
}

void onNext(Fl_Widget*, void*) {
    loadNextWord();
}

void onResetProgress(Fl_Widget*, void*) {
    words = ProgressManager::loadWordsFromBase("words.json");

    if (words.empty()) {
        wordBox->copy_label("Failed to load words.json");
        resultBox->copy_label("Reset failed.");
        suggestionBox->copy_label("");
        statsBox->copy_label("");
        progressBox->copy_label("");
        hardestBox->copy_label("");
        hintBox->copy_label("");
        return;
    }

    rebuildTrie();
    ProgressManager::saveProgress("progress.json", words);

    resultBox->copy_label("Progress reset to initial state.");
    suggestionBox->copy_label("");
    hardestBox->copy_label("");

    loadNextWord();
}

void onShowHardest(Fl_Widget*, void*) {
    if (words.empty()) {
        hardestBox->copy_label("No words available.");
        return;
    }

    std::vector<Word> difficultWords;
    for (const auto& w : words) {
        if (w.wrongCount > 0 || w.learningWeight > 5) {
            difficultWords.push_back(w);
        }
    }

    if (difficultWords.empty()) {
        hardestBox->copy_label("No difficult words yet.\nMake some mistakes first or continue learning.");
        return;
    }

    std::sort(difficultWords.begin(), difficultWords.end(),
        [](const Word& a, const Word& b) {
            if (a.learningWeight != b.learningWeight)
                return a.learningWeight > b.learningWeight;
            if (a.wrongCount != b.wrongCount)
                return a.wrongCount > b.wrongCount;
            if (a.masteryLevel != b.masteryLevel)
                return a.masteryLevel < b.masteryLevel;
            return a.serbian < b.serbian;
        });

    int limit = std::min(5, (int)difficultWords.size());

    std::string text = "Top hardest words / phrases:\n";
    for (int i = 0; i < limit; i++) {
        const Word& w = difficultWords[i];
        text += std::to_string(i + 1) + ". " +
                w.serbian + " -> " + w.english +
                " | W=" + std::to_string(w.learningWeight) +
                " | Wrong=" + std::to_string(w.wrongCount) +
                " | Level=" + graph.getLevelName(w.masteryLevel) + "\n";
    }

    hardestBox->copy_label(text.c_str());
}

void onSubmit(Fl_Widget*, void*) {
    if (currentIndex == -1) return;

    std::string user = toLower(inputBox->value());
    std::string correct = toLower(words[currentIndex].english);

    bool exactMatch = (user == correct);
    bool containsMatch = RabinKarp::search(user, correct);

    if (exactMatch || containsMatch) {

        if (exactMatch)
            resultBox->copy_label("Correct!");
        else
            resultBox->copy_label("Correct (expected answer found in sentence)!");

        suggestionBox->copy_label("");

        words[currentIndex].correctCount++;
        words[currentIndex].masteryLevel =
            graph.advanceOnCorrect(words[currentIndex].masteryLevel);

        words[currentIndex].learningWeight =
            graph.updateWeightOnCorrect(words[currentIndex].learningWeight);

        if (words[currentIndex].intervalDays == 1)
            words[currentIndex].intervalDays = 2;
        else
            words[currentIndex].intervalDays *= 2;

    } else {
        resultBox->copy_label("Wrong!");

        words[currentIndex].wrongCount++;
        words[currentIndex].masteryLevel =
            graph.degradeOnWrong(words[currentIndex].masteryLevel);

        words[currentIndex].learningWeight =
            graph.updateWeightOnWrong(words[currentIndex].learningWeight);

        words[currentIndex].intervalDays = 1;

        auto prefixCandidates = trie.getCandidatesByPrefix(user, 20);
        auto autoSuggestions =
            SuggestionHelper::rankAutocompleteSuggestions(user, prefixCandidates, words, 3);

        std::string text = "Suggestions:\n";

        if (!autoSuggestions.empty()) {
            for (auto& s : autoSuggestions) {
                text += "- " + s + "\n";
            }
        } else {
            auto fuzzySuggestions =
                SuggestionHelper::getTopFuzzySuggestions(user, words, 3, 2);

            if (!fuzzySuggestions.empty()) {
                for (auto& s : fuzzySuggestions) {
                    text += "- " + s + "\n";
                }
            } else {
                text += "(no suggestions)";
            }
        }

        suggestionBox->copy_label(text.c_str());
    }

    words[currentIndex].nextReviewTime =
        std::time(nullptr) + words[currentIndex].intervalDays * 24 * 60 * 60;

    ProgressManager::saveProgress("progress.json", words);

    updateStatsBox();
}

int main() {
    words = ProgressManager::loadProgressOrBase("words.json", "progress.json");
    rebuildTrie();

    Fl_Window* window = new Fl_Window(760, 610, "Language Learning App");

    Fl_Box* title = new Fl_Box(0, 10, 760, 30, "Vocabulary and Phrase Trainer");
    title->labelfont(FL_BOLD);
    title->labelsize(20);

    wordBox = new Fl_Box(50, 60, 660, 30, "");
    wordBox->labelsize(16);

    hintBox = new Fl_Box(50, 95, 660, 20, "");
    hintBox->labelsize(12);
    hintBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    inputBox = new Fl_Input(170, 125, 380, 35);
    inputBox->textsize(16);

    Fl_Button* submit = new Fl_Button(60, 180, 110, 40, "Submit");
    submit->callback(onSubmit);

    nextButton = new Fl_Button(200, 180, 110, 40, "Next");
    nextButton->callback(onNext);

    resetButton = new Fl_Button(340, 180, 110, 40, "Reset");
    resetButton->callback(onResetProgress);

    hardestButton = new Fl_Button(480, 180, 170, 40, "Hardest Words");
    hardestButton->callback(onShowHardest);

    resultBox = new Fl_Box(50, 240, 660, 30, "");
    resultBox->labelsize(16);

    statsBox = new Fl_Box(50, 275, 660, 30, "");
    statsBox->labelsize(14);
    statsBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    progressBox = new Fl_Box(50, 305, 660, 25, "");
    progressBox->labelsize(14);
    progressBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    suggestionBox = new Fl_Box(50, 335, 660, 95, "");
    suggestionBox->labelsize(14);
    suggestionBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);

    hardestBox = new Fl_Box(50, 440, 660, 145, "");
    hardestBox->labelsize(13);
    hardestBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE | FL_ALIGN_TOP);

    window->end();
    window->show();

    loadNextWord();

    return Fl::run();
}