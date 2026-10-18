#include <iostream>
#include "parser.hpp"
#include "graph.hpp"
#include "algoritmi.hpp"
#include "petlja.hpp"

int main(int argc, char* argv[]) {
    std::string registarFajl = "packages.txt";
    if (argc > 1) registarFajl = argv[1];

    std::cout << "=====================================================\n";
    std::cout << "         PACKAGE DEPENDENCY ANALYZER\n";
    std::cout << "=====================================================\n\n";

    std::vector<Ivica> ivice;
    std::vector<std::string> sviPaketi;
    if (!parsirajRegistar(registarFajl, ivice, sviPaketi)) return 1;

    Graf graf;
    graf.popuni(ivice, sviPaketi);

    std::cout << "Loaded: " << graf.brojPaketa()
          << " packages, " << ivice.size() << " dependencies.\n";
    // Ciklusi se proveravaju odmah nad celim registrom
    detektujCikluse(graf);

    pokreniPetlju(graf);

    return 0;
}
