#ifndef ALGORITMI_HPP
#define ALGORITMI_HPP
#include "graph.hpp"
#include <vector>
#include <string>

// 1. DETEKCIJA KRUŽNIH ZAVISNOSTI (Tarjan SCC)
void detektujCikluse(const Graf& graf);

// 2. REDOSLED INSTALACIJE (Kahn — topološko sortiranje)
//    Daje redosled u kome treba instalirati pakete iz datog skupa
//    id-ova, tako da zavisnosti uvek dolaze pre paketa koji ih koristi.
//    Ako skup nije zadat (prazan vektor), radi na celom grafu.
void redosledInstalacije(const Graf& graf,
                         const std::vector<int>& podskup = {});

// 3. TRANZITIVNE ZAVISNOSTI (BFS)
//    Za dati paket vraca sve pakete koji ce biti instalirani
//    zajedno s njim (direktne i indirektne zavisnosti).
std::vector<int> tranzitivneZavisnosti(const Graf& graf, int startId);

// 4. IZOLOVANI PAKETI
//    Paketi koji nemaju nikakve zavisnosti niti ko zavisi od njih.
//    Izolovanost svojstvo celog grafa,
void izolovaniPaketi(const Graf& graf);

#endif 
