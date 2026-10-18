#ifndef KOMANDE_HPP
#define KOMANDE_HPP
#include "graph.hpp"
#include <string>
#include <vector>

// Pomocne funkcije 

// Stampa listu paketa po nazivu; ako je prazna, ispisuje "(nema)"
void stampaListu(const Graf& graf, const std::vector<int>& ids);



// Skuplja id-ove paketa 'naziv' i svih njegovih tranzitivnih zavisnosti.
// Vraca false ako paket nije u registru.
bool skupiPodskup(const Graf& graf,
                  const std::string& naziv,
                  std::vector<int>& podskup);



// Komande — svaka odgovara jednoj korisnickoj komandi

// install <paket> — redosled instalacije paketa i svih zavisnosti
void komandaInstall(const Graf& graf, const std::string& naziv);



// deps <paket> — sve tranzitivne zavisnosti paketa
void komandaDeps(const Graf& graf, const std::string& naziv);



// info <paket> — direktne zavisnosti paketa
void komandaInfo(const Graf& graf, const std::string& naziv);



// needed <fajl> — ucitava needed fajl i prikazuje redosled instalacije
void komandaNeeded(const Graf& graf, const std::string& putanja);

//load <fajl> - ucitava podatke iz registra i smesta u graf
void komandaLoad(Graf &graf, std::string& putanja);

#endif 