#ifndef PETLJA_HPP
#define PETLJA_HPP
#include "graph.hpp"

/* Pokrece interaktivnu petlju koja cita komande sa standardnog ulaza
 i izvrsava ih nad datim grafom.
 Petlja se zavrsava komandom 'quit', 'exit' ili EOF-om.*/

void pokreniPetlju(Graf& graf);

#endif 
