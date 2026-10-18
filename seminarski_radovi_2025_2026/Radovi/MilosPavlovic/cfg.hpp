#ifndef CFG_HPP
#define CFG_HPP
#include <vector>
#include <unordered_map>
#include <string>
#include<stack>
#include "parser.hpp"
#include "instrukcija.hpp"

extern string start;

using namespace std;


vector<vector<int>> napraviCFG(vector<Instrukcija>& instrukcije, unordered_map<string,int>& mapaLabela); 

void Tarjan(int cvor, vector<vector<int>>& listaSusjedstva, vector<int>& dolazna, vector<int>& lowlink,
            vector<bool>& naSteku, stack<int>& stek, vector<vector<int>>& komponente);

void analizaKoda(vector<Instrukcija>& instrukcije, unordered_map<string,int>& mapaLabela); 

void terminira(vector<Instrukcija>& instrukcije, vector<int>& dolazna);

void nedostizanKod(vector<Instrukcija>& instrukcije, vector<int>& nedostizne);

void petlje(vector<Instrukcija>& instrukcije, vector<vector<int>>& komponente);

#endif 
