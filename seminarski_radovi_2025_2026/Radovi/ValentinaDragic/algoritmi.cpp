#include "algoritmi.hpp"
#include <iostream>
#include <queue>
#include <stack>

// 1. TARJAN SCC — detekcija kružnih zavisnosti

static void tarjanDFS(int cvor,
                      const Graf& graf,
                      int& brojac,
                      std::vector<int>& preorder,
                      std::vector<int>& lowlink,
                      std::vector<bool>& onStack,
                      std::stack<int>& stack,
                      std::vector<std::vector<int>>& components) {

    preorder[cvor] = brojac;
    lowlink[cvor]    = brojac;
    brojac++;

    onStack[cvor] = true;
    stack.push(cvor);

    for (int sused : graf.susedi(cvor)) {
        if (preorder[sused] == -1) {
            tarjanDFS(sused, graf, brojac, preorder, lowlink, onStack, stack, components);
            lowlink[cvor] = std::min(lowlink[cvor], lowlink[sused]);
        } else if (onStack[sused]) {
            lowlink[cvor] = std::min(lowlink[cvor], preorder[sused]);
        }
    }

    if (lowlink[cvor] == preorder[cvor]) {
        std::vector<int> komponenta;
        while (true) {
            int vrh = stack.top(); stack.pop();
            onStack[vrh] = false;
            komponenta.push_back(vrh);
            if (vrh == cvor) break;
        }
        components.push_back(komponenta);
    }
}

void detektujCikluse(const Graf& graf) {
    int n = graf.brojPaketa();
    int brojac = 0;

    std::vector<int>  preorder(n, -1);
    std::vector<int>  lowlink(n, 0);
    std::vector<bool> onStack(n, false);
    std::stack<int>   stack;
    std::vector<std::vector<int>> components;

    for (int i = 0; i < n; i++) {
        if (preorder[i] == -1) {
            tarjanDFS(i, graf, brojac, preorder, lowlink, onStack, stack, components);
        }
    }

    std::vector<std::vector<int>> ciklusi;
    for (auto& k : components) {
        if (k.size() > 1) ciklusi.push_back(k);
    }

    if (ciklusi.empty()) {
        std::cout << "  No circular dependencies found. Graph is acyclic (DAG).\n";
    } else {
        std::cout << "  WARNING: Found " << ciklusi.size() << " circular dependency groups!\n";
        int br = 1;
        for (auto& c : ciklusi) {
            std::cout << "  Cycle " << br++ << ": ";
            for (int i = (int)c.size() - 1; i >= 0; i--) {
                std::cout << graf.paket(c[i]).naziv;
                if (i > 0) std::cout << " -> ";
            }
            std::cout << "\n";
        }
    }
    std::cout << "\n";
}

// 2. KAHN — topološko sortiranje

// Pomocna: pravi inverzan graf samo za dati podskup id.
// Ako je podskup prazan, radi na celom grafu.
static std::vector<int> kahnSort(const Graf& graf,
                                  const std::vector<int>& podskup) {

    // Odredjujemo skup id-ova na kojima radimo
    std::vector<bool> ukljucen(graf.brojPaketa(), false);
    if (podskup.empty()) {
        std::fill(ukljucen.begin(), ukljucen.end(), true);
    } else {
        for (int id : podskup) ukljucen[id] = true;
    }

    // Racunamo broj zavisnosti koje su u skupu (preostalo[u])
    std::vector<int> preostalo(graf.brojPaketa(), 0);
    std::vector<std::vector<int>> invertovan(graf.brojPaketa());

    for (int u = 0; u < graf.brojPaketa(); u++) {
        if (!ukljucen[u]) continue;
        for (int v : graf.susedi(u)) {
            if (!ukljucen[v]) continue;
            preostalo[u]++;
            invertovan[v].push_back(u);
        }
    }

    std::queue<int> red;
    for (int i = 0; i < graf.brojPaketa(); i++) {
        if (ukljucen[i] && preostalo[i] == 0) red.push(i);
    }

    std::vector<int> redosled;
    while (!red.empty()) {
        int cvor = red.front(); red.pop();
        redosled.push_back(cvor);
        for (int zavisnik : invertovan[cvor]) {
            if (--preostalo[zavisnik] == 0) red.push(zavisnik);
        }
    }

    return redosled;
}

void redosledInstalacije(const Graf& graf, const std::vector<int>& podskup) {
    std::vector<int> redosled = kahnSort(graf, podskup);

    int ocekivano = podskup.empty() ? graf.brojPaketa() : (int)podskup.size();

    if ((int)redosled.size() != ocekivano) {
        std::cout << "  ERROR: Cannot determine installation order!\n";
        std::cout << "  Circular dependencies prevent installation.\n";
    } else {
        for (int i = 0; i < (int)redosled.size(); i++) {
            std::cout << "  " << (i+1) << ". " << graf.paket(redosled[i]).naziv << "\n";
        }
    }
    std::cout << "\n";
}

// ---------------------------------------------------------------
// 3. BFS — tranzitivne zavisnosti
// ---------------------------------------------------------------

std::vector<int> tranzitivneZavisnosti(const Graf& graf, int startId) {
    std::vector<bool> posecen(graf.brojPaketa(), false);
    std::queue<int> red;

    posecen[startId] = true;
    red.push(startId);

    std::vector<int> rezultat;

    while (!red.empty()) {
        int cvor = red.front(); 
        red.pop();
        for (int sused : graf.susedi(cvor)) {
            if (!posecen[sused]) {
                posecen[sused] = true;
                red.push(sused);
                rezultat.push_back(sused);
            }
        }
    }

    return rezultat;
}

// ---------------------------------------------------------------
// 4. IZOLOVANI PAKETI
// ---------------------------------------------------------------

void izolovaniPaketi(const Graf& graf) {
    int n = graf.brojPaketa();

    

    std::vector<bool> imaVezu(n, false);
    for (int u = 0; u < n; u++) {
        for (int v : graf.susedi(u)) {
            imaVezu[u] = true;
            imaVezu[v] = true;
        }
    }

    bool nadjeni = false;
    for (int i = 0; i < n; i++) {
        if (!imaVezu[i]) {
            std::cout << "  - " << graf.paket(i).naziv << "\n";
            nadjeni = true;
        }
    }
    if (!nadjeni) std::cout << "  No isolated packages.\n";
    std::cout << "\n";
}
