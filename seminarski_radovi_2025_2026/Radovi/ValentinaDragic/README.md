# Dependency Package Analyzer

Konzolni alat za analizu zavisnosti softverskih paketa zasnovan na teoriji grafova.

Projekat modeluje skup paketa kao usmereni graf i omogućava analizu zavisnosti kroz klasične grafovske algoritme.

---

##  Ideja projekta

U savremenim softverskim sistemima, paketi zavise jedni od drugih (npr. Python, Node.js, Java ekosistemi).  
Ove zavisnosti se prirodno modeluju kao usmereni graf:

- Čvor → paket
- Ivica `A → B` → paket A zavisi od paketa B

Cilj projekta je analiza tog grafa i rešavanje tipičnih problema zavisnosti.

---

##  Implementirane funkcionalnosti

- Detekcija kružnih zavisnosti
- Topološko sortiranje (redosled instalacije)
- Pronalaženje tranzitivnih zavisnosti
- Analiza izolovanih paketa
- Obrada liste potrebnih paketa

---

##  Korišćeni algoritmi

### Tarjan SCC
Detekcija jako povezanih komponenti i kružnih zavisnosti.

### Kahn algoritam
Topološko sortiranje za određivanje redosleda instalacije.

### BFS (Breadth-First Search)
Pronalaženje svih tranzitivnih zavisnosti paketa.

---

##  Struktura projekta

```text
.
├── main.cpp
├── paket.hpp
├── graph.hpp / graph.cpp
├── parser.hpp / parser.cpp
├── algoritmi.hpp / algoritmi.cpp
├── komande.hpp / komande.cpp
├── petlja.hpp / petlja.cpp
├── packages.txt
├── needed.txt
└── README.md
```
##  Pokretanje

Projekat koristi Makefile za automatsko kompajliranje.

### Kompajliranje i pokretanje

```bash
make
./dep-analyzer
```
