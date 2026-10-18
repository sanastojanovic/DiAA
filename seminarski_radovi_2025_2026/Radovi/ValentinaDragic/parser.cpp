#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

// Uklanja razmake s pocetka i kraja stringa

static std::string ukloniRazmake(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Deli string po zarezu i preskace prazne tokene

static std::vector<std::string> podeliZarezom(const std::string& s) {
    std::vector<std::string> rezultat;
    std::stringstream ss(s);
    std::string token;
    while (std::getline(ss, token, ',')) {
        std::string t = ukloniRazmake(token);
        if (!t.empty()) rezultat.push_back(t);
    }
    return rezultat;
}

// Preskace prazne linije i komentare

static bool nebitnaLinija(const std::string& line) {
    return line.empty() || line[0] == '#';
}

// Parsira jedan red registra oblika "naziv: dep1, dep2, ..."
// Upisuje paket u allPackages i ivice u listu ivica.
// Vraca false ako line nije validna.
static bool rasclaniRedRegistra(const std::string& line,
                                 std::vector<Ivica>& edges,
                                 std::vector<std::string>& allPackages) {
    size_t dvotacka = line.find(':');

    // linija bez dvotacke: paket bez zavisnosti (samo naziv)
    std::string naziv = ukloniRazmake(
        dvotacka == std::string::npos ? line : line.substr(0, dvotacka)
    );

    if (naziv.empty()) return false;

    allPackages.push_back(naziv);

    if (dvotacka == std::string::npos) return true;

    // Desna strana: lista zavisnosti odvojena zarezima
    std::string desna = ukloniRazmake(line.substr(dvotacka + 1));
    if (desna.empty()) return true;

    for (const std::string& dep : podeliZarezom(desna)) {
        edges.push_back({naziv, dep});
    }

    return true;
}

// Ucitava registar zavisnosti
bool parsirajRegistar(const std::string& path,
                      std::vector<Ivica>& edges,
                      std::vector<std::string>& allPackages) {
    std::ifstream fajl(path); //odjednom pravi objekat za citanje fajla i pokusava da otvori fajl
    if (!fajl.is_open()) {
        std::cerr << "Cannot open file '" << path << "'\n";
        return false;
    }

    std::string line;
    while (std::getline(fajl, line)) {
        line = ukloniRazmake(line);
        if (nebitnaLinija(line)) continue;
        rasclaniRedRegistra(line, edges, allPackages);
    }

    return true;
}

// Ucitava listu potrebnih paketa
bool parsirajPotrebne(const std::string& path,
                      std::vector<std::string>& potrebni) {
    std::ifstream fajl(path);
    if (!fajl.is_open()) {
        std::cerr << "Cannot open file '" << path << "'\n";
        return false;
    }

    std::string line;
    while (std::getline(fajl, line)) {
        line = ukloniRazmake(line);
        if (nebitnaLinija(line)) continue;
        potrebni.push_back(line);
    }

    return true;
}
