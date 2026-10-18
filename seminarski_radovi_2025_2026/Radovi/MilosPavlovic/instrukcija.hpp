#ifndef INSTRUKCIJA_HPP
#define INSTRUKCIJA_HPP

#include<iostream>
#include<string>

using namespace std;

struct Instrukcija{

    string vrsta;
    int linija; // linija u kojoj se nalazi instrukcija
    string labela; // ovo je opciono (samo za instrukcije skoka)

    bool bezuslovniSkok() const{
        return vrsta == "jmp";
    }

    bool uslovniSkok() const{
        return (vrsta == "je" || vrsta == "jg" || vrsta == "jl" || vrsta == "jle"
                || vrsta == "jne" || vrsta == "jae" || vrsta == "jge");
    }

    bool Skok() const{
        return (bezuslovniSkok() || uslovniSkok()); 
    }

    bool izlaz() const{
        return vrsta == "ret";
    }

};


#endif
