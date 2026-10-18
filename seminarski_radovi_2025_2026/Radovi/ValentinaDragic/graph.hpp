#ifndef GRAPH_HPP
#define GRAPH_HPP
#include "paket.hpp"
#include "parser.hpp"
#include <vector>
#include <unordered_map>
#include <string>

// Graf zavisnosti paketa.
// Cvor = paket (naziv + id), ivica u->v znaci "u zavisi od v".
class Graf {
public:
    // Puni graf iz edge liste i liste svih paketa iz registra.
    // Paketi koji se pojavljuju samo kao zavisnosti (bez sopstvenog reda
    // u registru) se takodje dodaju kao cvorovi.
    void popuni(const std::vector<Ivica>& ivice,
                const std::vector<std::string>& sviPaketi);

    // Geteri — sav ostali kod radi iskljucivo sa id-jevima
    int brojPaketa() const { 
        return (int)paketi_.size(); 
    }
    const Paket& paket(int id) const { 
        return paketi_[id]; 
    }
    const std::vector<int>& susedi(int id) const { 
        return neighbours_[id]; 
    }
    int nadjiId(const std::string& naziv) const;

private:
    std::vector<Paket> paketi_;
    std::vector<std::vector<int>> neighbours_;
    std::unordered_map<std::string, int> idx_;

    int dohvatiIliDodaj(const std::string& naziv);
};

#endif 
