#include "petlja.hpp"
#include "komande.hpp"
#include "algoritmi.hpp"
#include <iostream>
#include <sstream>

static void printHelp() {
    std::cout << "\nOptions:\n";
    std::cout << "  install <package>     show installation order for package and dependencies\n";
    std::cout << "  deps    <package>     list transitive dependencies\n";
    std::cout << "  info    <package>     show direct dependencies\n";
    std::cout << "  load    <registry>    load registry again in case of change\n";
    std::cout << "  cycles                detect circular dependencies\n";
    std::cout << "  isolated              show isolated packages\n";
    std::cout << "  needed  <file>        load needed file and show installation order\n";
    std::cout << "  help                  show available commands\n";
    std::cout << "  quit                  exit program\n\n";
}
void pokreniPetlju( Graf& graf) {
    printHelp();

    std::string linija;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, linija)) break;

        size_t razmak = linija.find(' ');

        std::string komanda;
        std::string argument;

        if (razmak != std::string::npos) {
            komanda = linija.substr(0, razmak);
            argument = linija.substr(razmak + 1);
        } else {
            komanda = linija;
        }

        if (komanda == "quit" || komanda == "exit") {
            break;
        } else if (komanda == "install") {
            if (argument.empty()) std::cerr << "Missing argument. Use: install <package>\n";
            else komandaInstall(graf, argument);
        } else if (komanda == "deps") {
            if (argument.empty()) std::cerr << "Missing argument. Use: deps <package>\n";
            else komandaDeps(graf, argument);
        } else if (komanda == "info") {
            if (argument.empty()) std::cerr << "Missing argument. Use: info <package>\n";
            else komandaInfo(graf, argument);
        } else if (komanda == "cycles") {
            detektujCikluse(graf);
        } else if (komanda == "isolated") {
            izolovaniPaketi(graf);
        } else if (komanda == "needed") {
            if (argument.empty()) std::cerr << "Missing argument. Use: needed <file>\n";
            else komandaNeeded(graf, argument);
        } else if (komanda == "help") {
            printHelp();
        }else if (komanda == "load") {
            komandaLoad(graf, argument);
        } else if (!komanda.empty()) {
            std::cerr << "Unknown command: '" << komanda
                      << "'. Type 'help' to see available commands.\n";
        }
    }
}
