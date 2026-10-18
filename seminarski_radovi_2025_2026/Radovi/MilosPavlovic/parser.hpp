#ifndef PARSER_HPP
#define PARSER_HPP

#include<string>
#include<vector>
#include<unordered_map>
#include "instrukcija.hpp"

using namespace std;

extern string start;

// glavna stvar koju parser treba da vrati je niz instrukcija zajedno sa mapom labela
// pomocu koje cemo dalje da iskonstruisemo graf kontrole toka


//glavna stvar
pair<vector<Instrukcija>, unordered_map<string, int>> parsirajFajl(const string imeFajla);
 
bool labela(string& linija); 

bool instrukcija(string& linija);

bool instrukcijaSkoka(string& linija);

bool direktiva(string& linija);

string imeLabele(string& linija);

string vrstaInstrukcije(string& linija);

string odsjeciBjeline(string& linija);

string imeInstrukcije(string& linija);

string izdvojLabelu(string& linija);


#endif
