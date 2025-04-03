#include <iostream>

using namespace std;

#define BRAK_KOLORU -1
#define NIEBIESKI 0
#define CZERWONY 1


////////// struktura kolejki //

struct ElementKolejki
{
    int identyfikator;
    ElementKolejki* nastepny;
};

struct Kolejka
{
    ElementKolejki* poczatek;
    ElementKolejki* koniec;
};

void dodaj_do_kolejki(Kolejka& kolejka, int wartosc)
{
    ElementKolejki* nowy_element = new ElementKolejki;
    nowy_element->identyfikator = wartosc;
    nowy_element->nastepny = nullptr;

    if (kolejka.poczatek == nullptr)
    {
        kolejka.poczatek = nowy_element;
        kolejka.koniec = nowy_element;
    }
    else 
    {
        kolejka.koniec->nastepny = nowy_element;
        kolejka.koniec = nowy_element;
    }
}

int usun_z_kolejki(ElementKolejki*& poczatek)
{
    if (poczatek == nullptr)
        return -1;

    int wartosc = poczatek->identyfikator;
    poczatek = poczatek->nastepny;

    return wartosc;
}


////////// STRUKTURA GRAFU //

struct Graf
{
    int liczba_wierzcholkow;
    Kolejka** lista_sasiedztwa;
};

void inicjalizuj_graf(Graf& graf, int n)
{
    graf.liczba_wierzcholkow = n;
    graf.lista_sasiedztwa = new Kolejka * [n];

    for (int i = 0; i < n; i++)
    {
        graf.lista_sasiedztwa[i] = new Kolejka;
        graf.lista_sasiedztwa[i]->poczatek = nullptr;
        graf.lista_sasiedztwa[i]->koniec = nullptr;    
    }
}

void wczytaj_liste_sasiedztwa(Graf& graf)
{
    for (int i = 0; i < graf.liczba_wierzcholkow; i++)
    {
        int s, sasiad;
        cin >> s;
        for (int j = 0; j < s; j++)
        {
            cin >> sasiad;
            dodaj_do_kolejki(*(graf.lista_sasiedztwa[i]), sasiad-1);
        }
    }
}

void zwolnij_pamiec(Graf& graf)
{
    for (int i = 0; i < graf.liczba_wierzcholkow; i++)
    {
        while (graf.lista_sasiedztwa[i]->poczatek != nullptr)
            usun_z_kolejki((graf.lista_sasiedztwa[i])->poczatek);

        delete graf.lista_sasiedztwa[i];
    }

    delete[] graf.lista_sasiedztwa;
}


////////// WYZNACZANE PARAMETRÓW //

// 1.
void zamien_miejscami(int tablica[], int i_1, int i_2)
{
    int temp = tablica[i_1];
    tablica[i_1] = tablica[i_2];
    tablica[i_2] = temp;
}

void heapify(int tablica[], int n, int i)
{
    int najmniejszy = i;
    int lewy = 2 * i + 1;
    int prawy = 2 * i + 2;

    if (lewy < n && tablica[lewy] < tablica[najmniejszy])
        najmniejszy = lewy;

    if (prawy < n && tablica[prawy] < tablica[najmniejszy])
        najmniejszy = prawy;

    if (najmniejszy != i)
    {
        zamien_miejscami(tablica, i, najmniejszy);
        heapify(tablica, n, najmniejszy);
    }
}

void heapsort(int tablica[], int n)
{
    for (int i = n/2 - 1; i >= 0; i--)
        heapify(tablica, n, i);

    for (int i = n-1; i > 0; i--)
    {
        zamien_miejscami(tablica, 0, i);
        heapify(tablica, i, 0);
    }
}

void ciag_stopniowy(Graf& graf)
{
    int* ciag_stopniowy = new int[graf.liczba_wierzcholkow];

    for (int i = 0; i < graf.liczba_wierzcholkow; i++) 
    {
        ciag_stopniowy[i] = 0;
        ElementKolejki* sasiad = graf.lista_sasiedztwa[i]->poczatek;
        while (sasiad != nullptr) {
            ciag_stopniowy[i]++;
            sasiad = sasiad->nastepny;
        }
    }

    heapsort(ciag_stopniowy, graf.liczba_wierzcholkow);

    for (int i = 0; i < graf.liczba_wierzcholkow; i++)
        cout << ciag_stopniowy[i] << " ";

    cout << endl;
    delete[] ciag_stopniowy;
}

// 2.
void bfs(Graf& graf, int start, bool* odwiedzone)
{
    Kolejka kolejka = { nullptr, nullptr };
    dodaj_do_kolejki(kolejka, start);
    odwiedzone[start] = true;

    while (kolejka.poczatek != nullptr) 
    {
        int v = usun_z_kolejki(kolejka.poczatek);
        ElementKolejki* sasiad = graf.lista_sasiedztwa[v]->poczatek;
        while (sasiad != nullptr) 
        {
            if (!odwiedzone[sasiad->identyfikator]) 
            {
                odwiedzone[sasiad->identyfikator] = true;
                dodaj_do_kolejki(kolejka, sasiad->identyfikator);
            }
            sasiad = sasiad->nastepny;
        }
    }
}

int skladowe_spojnosci(Graf& graf)
{
    bool* odwiedzone = new bool[graf.liczba_wierzcholkow];
    for (int i = 0; i < graf.liczba_wierzcholkow; i++)
        odwiedzone[i] = false;

    int liczba_skladowych = 0;
    for (int i = 0; i < graf.liczba_wierzcholkow; i++)
    {
        if (!odwiedzone[i]) 
        {
            bfs(graf, i, odwiedzone);
            liczba_skladowych++;
        }
    }

    delete[] odwiedzone;
    return liczba_skladowych;
}


// 3.
bool kolorowanie_wierzcholkow(Graf& graf, int wierzcholek_startowy, int* kolory) 
{
    Kolejka kolejka = { nullptr, nullptr };
    dodaj_do_kolejki(kolejka, wierzcholek_startowy);
    kolory[wierzcholek_startowy] = NIEBIESKI;

    while (kolejka.poczatek != nullptr)
    {
        int wierzcholek = usun_z_kolejki(kolejka.poczatek);
        ElementKolejki* sasiad = graf.lista_sasiedztwa[wierzcholek]->poczatek;

        while (sasiad != nullptr) 
        {
            int sasiedni_wierzcholek = sasiad->identyfikator;
            if (kolory[sasiedni_wierzcholek] == BRAK_KOLORU) 
            { 
                kolory[sasiedni_wierzcholek] = 1 - kolory[wierzcholek]; // kolor przeciwny
                dodaj_do_kolejki(kolejka, sasiedni_wierzcholek);
            }
            else if (kolory[sasiedni_wierzcholek] == kolory[wierzcholek])
                return false; // brak dwudzielności
            
            sasiad = sasiad->nastepny;
        }
    }

    return true;
}

bool sprawdz_dwudzielnosc(Graf& graf)
{
    int n = graf.liczba_wierzcholkow;
    int* kolory = new int[n];

    for (int i = 0; i < n; i++) 
        kolory[i] = BRAK_KOLORU;

    for (int wierzcholek = 0; wierzcholek < n; wierzcholek++)
    {
        if (kolory[wierzcholek] == BRAK_KOLORU) 
        { 
            if (!kolorowanie_wierzcholkow(graf, wierzcholek, kolory)) 
            {
                delete[] kolory;
                return false; 
            }
        }
    }

    delete[] kolory;
    return true;
}




int main()
{
    int k;
    cin >> k;

    for (int i = 0; i < k; i++)
    {
        int liczba_wierzcholkow;
        cin >> liczba_wierzcholkow;

        Graf graf;
        inicjalizuj_graf(graf, liczba_wierzcholkow);
        wczytaj_liste_sasiedztwa(graf);

        ciag_stopniowy(graf);
        cout << skladowe_spojnosci(graf) << endl;

        if (sprawdz_dwudzielnosc(graf)) 
            cout << "T" << endl;
        else 
            cout << "F" << endl;

        // brak implementacji
        for (int j = 0; j < 7; j++)
            cout << "?" << endl;

        zwolnij_pamiec(graf);
    }

    return 0;
}
