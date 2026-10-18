#include "komande.hpp"
#include "algoritmi.hpp"
#include "parser.hpp"
#include <iostream>

// Pomocne funkcije

void stampajListu(const Graf& graf, const std::vector<int>& ids) {
    if (ids.empty()) {
        std::cout << "  (not)\n";
        return;
    }
    for (int id : ids)
        std::cout << "  - " << graf.paket(id).naziv << "\n";
}

bool napraviPodskup(const Graf& graf,
             const std::string& naziv,
             std::vector<int>& subset) {

    int id = graf.nadjiId(naziv);

    if (id == -1) {
        std::cerr << "Error: package '" << naziv
                  << "' not in registry.\n";
        return false;
    }

    subset = tranzitivneZavisnosti(graf, id);

    // dodaj i pocetni paket
    subset.push_back(id);

    return true;
}

// Komande

void komandaInstall(const Graf& graf, const std::string& naziv) {
    std::vector<int> subset;
    if (!napraviPodskup(graf, naziv, subset)) return;

    std::cout << "\nInstallation order for package '" << naziv << "':\n";
    redosledInstalacije(graf, subset);
}

void komandaDeps(const Graf& graf, const std::string& naziv) {
    int id = graf.nadjiId(naziv);
    if (id == -1) {
        std::cerr << "Error: package '" << naziv << "' not in the registry.\n";
        return;
    }

    std::vector<int> zavisnosti = tranzitivneZavisnosti(graf, id);
    std::cout << "\nTransitive dependencies for package '" << naziv
              << "' (" << zavisnosti.size() << "):\n";
    stampajListu(graf, zavisnosti);
    std::cout << "\n";
}

void komandaInfo(const Graf& graf, const std::string& naziv) {
    int id = graf.nadjiId(naziv);
    if (id == -1) {
        std::cerr << "Error: package '" << naziv << "' not in the registry.\n";
        return;
    }

    const std::vector<int>& direktne = graf.susedi(id);
    std::cout << "\nDirect dependencies for package '" << naziv
              << "' (" << direktne.size() << "):\n";
    stampajListu(graf, direktne);
    std::cout << "\n";
}

void komandaNeeded(const Graf& graf, const std::string& putanja) {
    std::vector<std::string> potrebni;
    if (!parsirajPotrebne(putanja, potrebni)) return;

    std::vector<bool> inSet(graf.brojPaketa(), false);

    for (const std::string& naziv : potrebni) {
        int id = graf.nadjiId(naziv);
        if (id == -1) {
            std::cerr << "Error: package '" << naziv << "' not in the registry.\n";
            return;
        }
        inSet[id] = true;
        for (int depId : tranzitivneZavisnosti(graf, id))
            inSet[depId] = true;
    }

    std::vector<int> subset;
    for (int i = 0; i < graf.brojPaketa(); i++)
        if (inSet[i]) subset.push_back(i);

    std::cout << "\nInstallation order (" << subset.size() << " packages):\n";
    redosledInstalacije(graf, subset);
}

void komandaLoad(Graf &graf, std::string& registarFajl){
    
    if (registarFajl == "") registarFajl = "packages.txt";
    // Parsiranje registra — greska ako fajl ne postoji
    std::vector<Ivica> ivice;
    std::vector<std::string> sviPaketi;
    if (!parsirajRegistar(registarFajl, ivice, sviPaketi)) {
        std::cerr<<" Could not load packages from "<< registarFajl << std::endl;
    };

    graf.popuni(ivice, sviPaketi);

    std::cout << "Loaded: " << graf.brojPaketa()
          << " packages, " << ivice.size() << " dependencies.\n";
}
