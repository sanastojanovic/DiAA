#include "parser.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <unordered_set>

string start;

using namespace std;

unordered_set<string> imenaInstrukcija = {
    "mov", "movzx", "add", "sub", "inc", "dec", "cmp", "je", "jne", "jg", "jge", "jl", "jle", "jae", "jmp", "div", "enter", "leave", "ret"
};

string odsjeciBjeline(string& linija){

    int pocetak = 0;
    int kraj = linija.size() - 1;

    while(pocetak < linija.size()){
        if(linija[pocetak] == ' ' || linija[pocetak] == '\t') pocetak++;
        else break;
    }

    while(kraj >= 0){
        if(linija[kraj] == ' ' || linija[kraj] == '\t') kraj--;
        else break;
    }

    if(pocetak > kraj) return "";

    return linija.substr(pocetak, kraj - pocetak + 1);
}

/////////////////////////////////////////////////////////////////////////

bool labela(string& linija){

    string lb = odsjeciBjeline(linija);
    if(lb.size() == 0) return false;

    if(lb[lb.size() - 1] == ':') return true;
    return false;
}

/////////////////////////////////////////////////////////////////////////

string imeLabele(string& labela){
    return labela.substr(0, labela.size() - 1);
}

/////////////////////////////////////////////////////////////////////////

bool instrukcija(string& linija){

    int index = 0;
    while(index < linija.size()){
        if(linija[index] == ' ' || linija[index] == '\t') break;
        index++;
    }

    string vrsta = linija.substr(0, index);

    auto it = imenaInstrukcija.find(vrsta);
    if(it != imenaInstrukcija.end()) return true;
    return false;
}

/////////////////////////////////////////////////////////////////////////

bool instrukcijaSkoka(string& linija){

    int index = 0;
    while(index < linija.size()){
        if(linija[index] == ' ' || linija[index] == '\t') break;
        index++;
    }

    string vrsta = linija.substr(0, index);

    if(vrsta == "jmp" || vrsta == "je" || vrsta == "jg" || vrsta == "jge" || 
       vrsta == "jl"  || vrsta == "jle" || vrsta == "jne" || vrsta == "jae") return true;

    return false;
}

/////////////////////////////////////////////////////////////////////////

bool direktiva(string& linija){

    linija = odsjeciBjeline(linija);
    if(linija == ".intel_syntax noprefix") return true;
    
    if(linija.starts_with(".section")) return true;

    if(linija == ".text") return true;

    if(linija.starts_with(".global")){

        int index = linija.size()-1;
        while(true){
            if(linija[index] == ' ' || linija[index] == '\t') break;
            else index--;
        }
        start = linija.substr(index+1, linija.size()-index);
        return true;
    }

    return false;
}


/////////////////////////////////////////////////////////////////////////

string imeInstrukcije(string& linija){

    int index = 0;
    while(index < linija.size()){
        if(linija[index] == ' ' || linija[index] == '\t') break;
        index++;
    }

    return linija.substr(0, index);
}

/////////////////////////////////////////////////////////////////////////

string izdvojLabelu(string& linija){

    int index = linija.size() - 1;

    while(index >= 0){
        if(linija[index] == ' ' || linija[index] == '\t') break;
        index--;
    }

    return linija.substr(index + 1);
}

/////////////////////////////////////////////////////////////////////////

pair<vector<Instrukcija>, unordered_map<string, int>> parsirajFajl(const string imeFajla){

    ifstream fajl(imeFajla);
    if(!fajl.is_open()){
        cerr << "Greska prilikom otvaranja fajla" << endl;
        exit(1);
    }

    vector<Instrukcija> instrukcije;
    //instrukcije.reserve(100);   // stabilne adrese

    unordered_map<string,int> mapaLabela;

    string linija;
    int broj_linije = 1;
    string trenutnaLabela;
    int indikator = 0;

    while(getline(fajl, linija)){

        // ako je linija prazna
        linija = odsjeciBjeline(linija);
        if(linija.size() == 0){
            broj_linije++;
            continue;
        }

        // sad idu provjere ako je neka od direktiva
        if(direktiva(linija)){
            broj_linije++;
            continue;
        }


        // ako je instrukcija
        if(instrukcija(linija)){

            Instrukcija instr;

            instr.vrsta = imeInstrukcije(linija);
            instr.linija = broj_linije;

            if(instrukcijaSkoka(linija)){
                instr.labela = izdvojLabelu(linija);
            }

            instrukcije.push_back(instr);

            if(indikator == 1){
                indikator = 0;
                mapaLabela[trenutnaLabela] = instrukcije.size()-1;
            }
        }
        else if(labela(linija)){
            trenutnaLabela = imeLabele(linija);
            indikator = 1;
        }
        else{
            cerr << "Greska u liniji broj: " << broj_linije << endl;
            exit(1);
        }

        broj_linije++;
    }

    return make_pair(instrukcije, mapaLabela);
}
