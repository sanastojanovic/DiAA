#include "graph.hpp"
#include <iostream>
#include <algorithm>

// Privatna pomocna funkcija: dodaje paket ako ne postoji i svakako vraca id 

int Graf::dohvatiIliDodaj(const std::string& naziv) {
    auto it = idx_.find(naziv);
    if (it != idx_.end()) return it->second;

    int id = (int)paketi_.size();
    paketi_.emplace_back(naziv, id);
    neighbours_.emplace_back(); //novom cvoru obezbedjujemo vektor u koji ce da smesta "komsije" ako kasnije naidjemo na njih
    idx_[naziv] = id;
    return id;
}

// Puni graf iz edge liste i liste svih paketa
void Graf::popuni(const std::vector<Ivica>& ivice,
                  const std::vector<std::string>& sviPaketi) {
    // Prvo registrujemo sve pakete iz registra kako bi paketi
    // bez zavisnosti takodje bili cvorovi u grafu
    for (const std::string& naziv : sviPaketi) {
        dohvatiIliDodaj(naziv);
    }

    // Zatim dodajemo ivice (i eventualne pakete koji se pojavljuju
    // samo kao zavisnosti, a nisu imali sopstveni red u registru)
    for (const Ivica& ivica : ivice) {
        int idOd = dohvatiIliDodaj(ivica.od);
        int idDo = dohvatiIliDodaj(ivica.do_);

        // Izbegavamo duplikate
        auto& pomocni = neighbours_[idOd];
        if (std::find(pomocni.begin(), pomocni.end(), idDo) == pomocni.end()) {
            pomocni.push_back(idDo);
        }
    }
}

// Geter
int Graf::nadjiId(const std::string& naziv) const {
    auto it = idx_.find(naziv);
    if (it == idx_.end()) return -1;
    return it->second;
}
