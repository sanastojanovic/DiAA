# Statički analizator za x86-64 asembler

Ovaj projekat implementira statički analizator za x86-64 asembler programe.

Analizator analizira kontrolni tok programa kako bi identifikovao:
- Potencijalno beskonačne petlje
- Nedostižne instrukcije
- Tačke izlaza iz programa (dostupne `ret` instrukcije)

---

## Funkcionalnosti

- Detekcija globalne ulazne tačke iz `.global` direktiva
- Konstrukcija Grafa Kontrole Toka (CFG)
- Pronalaženje jakih povezanih komponenti (SCC) radi detekcije potencijalno beskonačnih petlji
- Prikaz nedostižnog koda
- Upozorenje ako program nema dostupne `ret` instrukcije
- Modularna analiza kroz funkcije:
    - `terminira()` – pronalazi izlazne tacke programa
    - `petlje()` – detekcija potencijalno beskonačnih petlji
    - `nedostizanKod()` – pronalazi nedostizne instrukcije

---

## Zahtevi / Instalacija

- Potreban C++20 kompatibilan kompajler (npr. g++)
- Testirano na Linux sistemu
- Makefile je uključen i olakšava kompajliranje projekta

### Kompajliranje sa Makefile-om

U direktorijumu projekta pokrenite: make

## Pokretanje

- Nakon kompajliranja dobija se izvršni fajl a.out - program se pokreće sa ./a.out <ime_fajla>
