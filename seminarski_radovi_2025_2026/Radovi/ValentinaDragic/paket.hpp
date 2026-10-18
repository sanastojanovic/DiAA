#ifndef PAKET_HPP
#define PAKET_HPP
#include <string>

struct Paket {
    std::string naziv;
    int id;

    Paket() : naziv(""), id(-1) {}
    Paket(const std::string& naziv, int id) : naziv(naziv), id(id) {}
};

#endif 
