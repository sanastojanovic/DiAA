import heapq
import ipaddress


class TrieCvor:
    def __init__(self):
        self.deca = {}
        self.ruter = None
        self.prefiks = None


class IPTrie:
    def __init__(self):
        self.koren = TrieCvor()

    def ip_u_bitove(self, ip_adresa):
        broj = int(ipaddress.IPv4Address(ip_adresa))
        return format(broj, "032b")

    def dodaj_prefiks(self, mreza, ruter):
        # Mreza je oblika npr. "192.168.1.0/24"
        ip_mreza = ipaddress.IPv4Network(mreza, strict=False)
        bitovi = self.ip_u_bitove(str(ip_mreza.network_address))

        trenutni = self.koren
        for bit in bitovi[:ip_mreza.prefixlen]:
            if bit not in trenutni.deca:
                trenutni.deca[bit] = TrieCvor()
            trenutni = trenutni.deca[bit]

        trenutni.ruter = ruter
        trenutni.prefiks = str(ip_mreza)

    def nadji_najduzi_prefiks(self, ip_adresa):
        bitovi = self.ip_u_bitove(ip_adresa)
        trenutni = self.koren
        poslednji_pogodak = None

        for bit in bitovi:
            if bit not in trenutni.deca:
                break

            trenutni = trenutni.deca[bit]

            if trenutni.ruter is not None:
                poslednji_pogodak = (trenutni.prefiks, trenutni.ruter)

        return poslednji_pogodak


def dijkstra(graf, pocetak, kraj):
    red = [(0, pocetak, [pocetak])]
    poseceni = set()

    while red:
        cena, ruter, putanja = heapq.heappop(red)

        if ruter in poseceni:
            continue

        poseceni.add(ruter)

        if ruter == kraj:
            return cena, putanja

        for komsija, tezina in graf[ruter]:
            if komsija not in poseceni:
                nova_cena = cena + tezina
                nova_putanja = putanja + [komsija]
                heapq.heappush(red, (nova_cena, komsija, nova_putanja))

    return None, []


def prikazi_mrezu(graf):
    print("\nRuteri i veze u mrezi:")
    for ruter in graf:
        veze = []
        for komsija, cena in graf[ruter]:
            veze.append(f"{komsija}({cena})")
        print(f"  {ruter}: {', '.join(veze)}")


def napravi_primer_mreze():
    graf = {
        "R1": [("R2", 2), ("R3", 5)],
        "R2": [("R1", 2), ("R3", 1), ("R4", 4)],
        "R3": [("R1", 5), ("R2", 1), ("R4", 1), ("R5", 7)],
        "R4": [("R2", 4), ("R3", 1), ("R5", 3)],
        "R5": [("R3", 7), ("R4", 3)],
    }

    trie = IPTrie()
    trie.dodaj_prefiks("192.168.1.0/24", "R4")
    trie.dodaj_prefiks("192.168.2.0/24", "R5")
    trie.dodaj_prefiks("10.0.0.0/8", "R3")
    trie.dodaj_prefiks("172.16.0.0/16", "R2")

    return graf, trie


def simuliraj_rutiranje(graf, trie, pocetni_ruter, odredisna_ip):
    pogodak = trie.nadji_najduzi_prefiks(odredisna_ip)

    if pogodak is None:
        print("\nNema rute za tu IP adresu.")
        return

    prefiks, odredisni_ruter = pogodak
    cena, putanja = dijkstra(graf, pocetni_ruter, odredisni_ruter)

    if not putanja:
        print("\nRuter postoji u tabeli, ali ne postoji putanja kroz graf.")
        return

    print("\nRezultat rutiranja:")
    print(f"  Odredisna IP adresa: {odredisna_ip}")
    print(f"  Pronadjen prefiks: {prefiks}")
    print(f"  Mreza je povezana sa ruterom: {odredisni_ruter}")
    print(f"  Najkraca putanja: {' -> '.join(putanja)}")
    print(f"  Ukupna cena puta: {cena}")


def main():
    graf, trie = napravi_primer_mreze()
    prikazi_mrezu(graf)
    pocetni_ruter = input("\nUnesi pocetni ruter (R1-R5): ").strip().upper()
    if pocetni_ruter not in graf:
        print("Nepoznat pocetni ruter.")
        return

    odredisna_ip = input("Unesi odredisnu IP adresu: ").strip()

    try:
        ipaddress.IPv4Address(odredisna_ip)
    except ValueError:
        print("IP adresa nije ispravna.")
        return

    simuliraj_rutiranje(graf, trie, pocetni_ruter, odredisna_ip)


if __name__ == "__main__":
    main()
