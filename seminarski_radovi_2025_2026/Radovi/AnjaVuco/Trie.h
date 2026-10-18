#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>

class TrieNode {
public:
    std::unordered_map<char, std::unique_ptr<TrieNode>> children;
    bool isEndOfWord = false;
};

class Trie {
private:
    std::unique_ptr<TrieNode> root;

    void collectWords(const TrieNode* node,
                      const std::string& current,
                      std::vector<std::string>& results,
                      int limit) const {
        if (!node || (int)results.size() >= limit) {
            return;
        }

        if (node->isEndOfWord) {
            results.push_back(current);
        }

        std::vector<char> keys;
        for (const auto& pair : node->children) {
            keys.push_back(pair.first);
        }

        std::sort(keys.begin(), keys.end());

        for (char c : keys) {
            if ((int)results.size() >= limit) {
                break;
            }

            collectWords(node->children.at(c).get(), current + c, results, limit);
        }
    }

public:
    Trie() : root(std::make_unique<TrieNode>()) {}

    void clear() {
        root = std::make_unique<TrieNode>();
    }

    void insert(const std::string& word) {
        TrieNode* current = root.get();

        for (char c : word) {
            if (current->children.find(c) == current->children.end()) {
                current->children[c] = std::make_unique<TrieNode>();
            }
            current = current->children[c].get();
        }

        current->isEndOfWord = true;
    }

    bool search(const std::string& word) const {
        const TrieNode* current = root.get();

        for (char c : word) {
            auto it = current->children.find(c);
            if (it == current->children.end()) {
                return false;
            }
            current = it->second.get();
        }

        return current->isEndOfWord;
    }

    std::vector<std::string> getCandidatesByPrefix(const std::string& prefix, int limit = 20) const {
        const TrieNode* current = root.get();
        std::vector<std::string> results;

        for (char c : prefix) {
            auto it = current->children.find(c);
            if (it == current->children.end()) {
                return results;
            }
            current = it->second.get();
        }

        collectWords(current, prefix, results, limit);
        return results;
    }
};

#endif