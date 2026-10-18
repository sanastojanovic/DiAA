#ifndef PARSER_HPP
#define PARSER_HPP
#include <string>
#include <vector>

/*
 Parser podrzava dva formata:
 
  1) Registar zavisnosti (packages.txt):
       flask: werkzeug, jinja2, click
       jinja2: markupsafe
       werkzeug:
     Format: naziv_paketa: dep1, dep2, ...
     Paketi bez zavisnosti mogu imati praznu desnu stranu ili je izostaviti.
 
  2) Lista potrebnih paketa (needed.txt):
       flask
       pandas
     Jedan paket po redu, prazni redovi i komentari se ignorisu.
 
 */

// Jedna zavisnost iz registra: "od" zavisi od "do_"
struct Ivica {
    std::string od;
    std::string do_;
};

// Ucitava registar zavisnosti; vraca edge list i listu svih paketa
// koji se pojavljuju (ukljucujuci i one bez zavisnosti).
// Vraca false ako fajl ne moze da se otvori.
bool parsirajRegistar(const std::string& putanja,
                      std::vector<Ivica>& ivice,
                      std::vector<std::string>& svaPaketi);

// Ucitava listu potrebnih paketa.
// Vraca false ako fajl ne moze da se otvori.
bool parsirajPotrebne(const std::string& putanja,
                      std::vector<std::string>& potrebni);

#endif 
