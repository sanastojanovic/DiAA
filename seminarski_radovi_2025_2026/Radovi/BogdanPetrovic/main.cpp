#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <memory>

using namespace std;

typedef pair<int, int> Score;

struct Node {
    unordered_map<char, unique_ptr<Node>> children;
    vector<Score> cache;

    void update_cache(int freq, int id, size_t K) {
        pair<int, int> entry = {freq, id};

        for (auto it = cache.begin(); it != cache.end(); ++it) {
            if (it->second == id) {
                cache.erase(it);
                break;
            }
        }

        auto pos = cache.begin();
        while (pos != cache.end() && *pos > entry) {
            ++pos;
        }
        cache.insert(pos, entry);

        if (cache.size() > K) cache.pop_back();
    }
};

class Trie {
    unique_ptr<Node> root = make_unique<Node>();
    unordered_map<string, int> word_to_id;
    vector<string> id_to_word;
    unordered_map<int, int> freqs; // word_id -> total_freq
    int K;

public:
    Trie(int k) : K(k) {}

    void update(const string& word, int add_freq = 1) {
        if (word_to_id.find(word) == word_to_id.end()) {
            word_to_id[word] = id_to_word.size();
            id_to_word.push_back(word);
        }

        int id = word_to_id[word];
        freqs[id] += add_freq;
        int total = freqs[id];

        Node* curr = root.get();
        curr->update_cache(total, id, K);

        for (char c : word) {
            if (!curr->children[c]) curr->children[c] = make_unique<Node>();
            curr = curr->children[c].get();
            curr->update_cache(total, id, K);
        }
    }

    vector<string> search(const string& prefix) {
        Node* curr = root.get();
        for (char c : prefix) {
            if (curr->children.find(c) == curr->children.end()) return {};
            curr = curr->children[c].get();
        }
        
        vector<string> res;
        for (auto& p : curr->cache) res.push_back(id_to_word[p.second]);
        return res;
    }
};

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cerr << "Upotreba: " << argv[0] << " <recnik.txt> <test.txt> <K>" << endl;
        return 1;
    }
    
    Trie trie(stoi(argv[3]));
    ifstream dict(argv[1]), test(argv[2]);
    ofstream out("output.txt");

    string line, word; int f;
    while (getline(dict, line)) {
        stringstream ss(line);
        if (ss >> word >> f) trie.update(word, f);
    }

    while (getline(test, word)) {
        if (word.empty()) continue;
        out << word << ": ";
        auto res = trie.search(word);
        for (size_t i = 0; i < res.size(); ++i) 
            out << res[i] << (i == res.size() - 1 ? "" : ", ");
        out << endl;
    }
    
    out.close();

    return 0;
}