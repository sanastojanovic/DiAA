#include "cfg.hpp"
#include "parser.hpp"

vector<vector<int>> napraviCFG(vector<Instrukcija>& instrukcije, unordered_map<string,int>& mapaLabela){

    int brojInstrukcija = instrukcije.size();
    
    vector<vector<int>> listaSusjedstva(brojInstrukcija);

    // ovdje je bolje krenuti od i = 0 nego od i = pocetak 
    for(int i = 0; i < brojInstrukcija; i++){
        // moracemo posebno obraditi posljednju konstrukciju
        if(i == brojInstrukcija-1){
            if(instrukcije[i].Skok()){

                if(instrukcije[i].bezuslovniSkok()){
                    int narednaInstrukcija = mapaLabela[instrukcije[i].labela];
                    listaSusjedstva[i].push_back(narednaInstrukcija);
                }
                else{
                    listaSusjedstva[i].push_back(mapaLabela[instrukcije[i].labela]);
                }
            }
        }
        // ako nismo na posljednjoj instrukciji
        else{
            // ako je u pitanju instrukcija skoka
            if(instrukcije[i].Skok()){
                // u slucaju bezuslovnog skoka dodajemo samo ovo
                if(instrukcije[i].bezuslovniSkok()){
                    int narednaInstrukcija = mapaLabela[instrukcije[i].labela];
                    listaSusjedstva[i].push_back(narednaInstrukcija);
                }
                // slucaj uslovnog skoka (dodajemo 2 grane)
                else{
                    listaSusjedstva[i].push_back(i+1);
                    listaSusjedstva[i].push_back(mapaLabela[instrukcije[i].labela]);
                }
            }
            // ako je u pitanju ret funkcija
            else if(instrukcije[i].izlaz()){
                
            }
            // ako su u pitanju ostale funkcije
            else{
                listaSusjedstva[i].push_back(i+1);
            }
        }

    }

    return listaSusjedstva;

}

int dolazni_broj = 0;

void Tarjan(int cvor, vector<vector<int>>& listaSusjedstva, vector<int>& dolazna, vector<int>& lowlink,
            vector<bool>& naSteku, stack<int>& stek, vector<vector<int>>& komponente){

                dolazna[cvor] = dolazni_broj++;
                lowlink[cvor] = dolazna[cvor];
                
                naSteku[cvor] = true;
                stek.push(cvor);

                for(auto susjed: listaSusjedstva[cvor]){
                    
                    if(dolazna[susjed] == -1){
                        Tarjan(susjed, listaSusjedstva, dolazna, lowlink, naSteku, stek, komponente);
                        lowlink[cvor] = min(lowlink[cvor], lowlink[susjed]);
                    }
                    else if(naSteku[susjed]){
                        lowlink[cvor] = min(lowlink[cvor], dolazna[susjed]);
                    }
                }

                // ostaje jos izlazna obrada a to je provjeriti da li je tekuci cvor mozda cvor komponente
                if(lowlink[cvor] == dolazna[cvor]){
                    vector<int> komponenta;
                    while(true){
                        int vrh = stek.top();
                        stek.pop();
                        komponenta.push_back(vrh);
                        if(vrh == cvor) break;
                    }

                    komponente.push_back(komponenta);
                }
            }

void analizaKoda(vector<Instrukcija>& instrukcije, unordered_map<string,int>& mapaLabela){

    vector<vector<int>> listaSusjedstva = napraviCFG(instrukcije, mapaLabela);
    int brojInstrukcija = listaSusjedstva.size();
    vector<int> dolazna(brojInstrukcija, -1);
    vector<int> lowlink(brojInstrukcija);
    vector<bool> naSteku(brojInstrukcija, false);
    stack<int> stek;
    vector<vector<int>> komponente;

    int pocetniCvor = mapaLabela[start];

    Tarjan(pocetniCvor, listaSusjedstva, dolazna, lowlink, naSteku, stek, komponente);

    // izdvajamo komponente jake povezanosti sa barem 2 clana
    vector<vector<int>> ciklusi;
    for(int i = 0; i < komponente.size(); i++){
        if(komponente[i].size() > 1) ciklusi.push_back(komponente[i]);
    }


    // indeksi nedostiznih instrukcija
    vector<int> nedostizne;
    for(int i = 0; i < brojInstrukcija; i++){
        if(dolazna[i] == -1) nedostizne.push_back(i); 
    }


    terminira(instrukcije,dolazna);
    petlje(instrukcije,ciklusi);
    nedostizanKod(instrukcije,nedostizne);

}

void terminira(vector<Instrukcija>& instrukcije, vector<int>& dolazna){

    cout << "---------------------------------------------------------------------" << endl;
    cout << endl;

    int broj_izlaza = 0;
    int brojInstrukcija = instrukcije.size();
    vector<int> linijeIzlaza;
    for(int i = 0;  i < brojInstrukcija; i++){
        if(dolazna[i] != -1 && instrukcije[i].izlaz()){
            broj_izlaza++;
            linijeIzlaza.push_back(instrukcije[i].linija);
        }
    }

    if(broj_izlaza == 0){
        cout << "UPOZORENJE: Ne postoji dostizna ret instrukcija u kodu!" << endl;
    }
    else{
        cout << "Broj izlaznih tacaka programa: " << broj_izlaza << endl;
        cout << "Linije izlaznih tacaka: "; 
        for(int i = 0; i < linijeIzlaza.size()-1; i++) cout << linijeIzlaza[i] <<", ";
        cout << linijeIzlaza[linijeIzlaza.size()-1] << endl;
    }

    cout << endl;
}


void nedostizanKod(vector<Instrukcija>& instrukcije, vector<int>& nedostizne){

    cout << "---------------------------------------------------------------------" << endl;
    cout << endl;

    if(nedostizne.size() == 0){
        cout << "Nema instrukcija koje su sigurno nedostizne!" << endl;
    }
    else{
        cout << "Broj sigurno nedostiznih instrukcija: " << nedostizne.size() << endl; 
        cout << "Linije sigurno nedostiznih instrukcija: ";
        for(int i = 0; i < nedostizne.size()-1; i++) cout << instrukcije[nedostizne[i]].linija << ", ";
        cout << instrukcije[nedostizne[nedostizne.size()-1]].linija << endl;
    }

    cout << endl;
}

void petlje(vector<Instrukcija>& instrukcije, vector<vector<int>>& komponente){

    cout << "---------------------------------------------------------------------" << endl;
    cout << endl;

    if(komponente.size() == 0){
        cout << "Nema petlji u kodu!" <<endl;
    }
    else{
        cout << "Broj potencijalno beskonacnih petlji: " << komponente.size() << endl;
        int brojac = 1;
        for(int i = komponente.size()-1; i >= 0; i--){
            int pocetak = instrukcije[komponente[i][komponente[i].size()-1]].linija;
            int kraj = instrukcije[komponente[i][0]].linija;
            cout << brojac++ <<": Pocev od linije " << pocetak << " do linije " << kraj << endl;
        }
    }

    cout << endl;
}
