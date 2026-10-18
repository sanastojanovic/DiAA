#include<iostream>
#include "parser.hpp"
#include "instrukcija.hpp"
#include "cfg.hpp"

using namespace std;


int main(int argc, char** argv){

    if(argc < 2){
        cerr << "Upotreba: ./a.out ime_fajla" << endl;
        exit(1);
    }

    string imeFajla = argv[1];

    auto [instrukcije, mapaLabela] = parsirajFajl(imeFajla);

    analizaKoda(instrukcije, mapaLabela);
    
    return 0;
}
