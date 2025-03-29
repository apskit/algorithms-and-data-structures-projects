#include <stdio.h>
#include <stdlib.h>

#define LICZBA 0
#define OPERATOR 1
#define IF 73 // kody operacji
#define NEG 71
#define MIN 78
#define MAX 88
#define OPERACJE_FUNKCYJNE 3 // minimalny priorytet
#define NAWIAS_OTWIERAJACY 40


/////// STRUKTURA STOSU

struct element
{
	bool rodzaj_elementu;
	int liczba; // operand
	char dzialanie; // operator
	struct element* nastepny;
} *poczatek = NULL;

void push(bool rodzaj_elementu, int liczba, char dzialanie, struct element** ptr)
{
	struct element* p = (struct element*)malloc(sizeof(struct element));

	p->liczba = liczba;
	p->dzialanie = dzialanie;

	p->rodzaj_elementu = rodzaj_elementu;
	p->nastepny = *ptr;

	*ptr = p;
}

int pop(struct element** ptr, bool rodzaj_elementu)
{
	if (*ptr == NULL)
	{
		printf("BŁĄD - stos jest pusty\n");
		return -1;
	}

	struct element* p = *ptr;
	int liczba;
	char znak;

	if (rodzaj_elementu == LICZBA)
		liczba = p->liczba;
	else
		znak = p->dzialanie;

	if (p->nastepny == NULL) // jeden element
		*ptr = NULL;
	else // wiele elementow
		*ptr = p->nastepny;

	free(p);

	if (rodzaj_elementu == LICZBA)
		return liczba;
	else
		return znak;
}


/////// OBSLUGA STOSU

void drukuj_operacje(char kod_operacji, struct element* stos)
{
	if (kod_operacji == NEG)
		printf("N ");
	else if (kod_operacji == IF)
		printf("IF ");
	else if (kod_operacji == MIN)
		printf("MIN%d ", (stos->liczba + 1));
	else if (kod_operacji == MAX)
		printf("MAX%d ", (stos->liczba + 1));	
	else
		printf("%c  ", kod_operacji);
}

void drukuj_stos(struct element* ptr)
{
	if (ptr == NULL)
		return;

	drukuj_stos(ptr->nastepny);

	if (ptr->rodzaj_elementu == LICZBA)
		printf("%d  ", ptr->liczba);
	else
	{
		char znak = ptr->dzialanie;
		drukuj_operacje(znak, ptr);
	}
}

void drukuj_stos_odwrocony(struct element* ptr)
{
	while (ptr != NULL)
	{
		if (ptr->rodzaj_elementu == LICZBA)
			printf("%d  ", ptr->liczba);
		else
		{
			char znak = ptr->dzialanie;
			drukuj_operacje(znak, ptr);
		}

		ptr = ptr->nastepny;
	}
	printf("\n");
}

void wyczysc_stos(struct element** ptr)
{
	while (*ptr != NULL)
		pop(ptr, 0);
}


/////// ODCZYT INPUTU

int odczytaj_liczbe_z_ciagu(char* ciag_znakow, int* j)
{
	int liczba = 0, i = 0;

	while (ciag_znakow[i] != ' ') {
		if (ciag_znakow[i] >= '0' && ciag_znakow[i] <= '9')
			liczba = liczba * 10 + (ciag_znakow[i] - '0');
		else
			break;
		i++;
	}

	(*j) += i;
	return liczba;
}

char odczytaj_operacje_z_ciagu(char* ciag_znakow, int* j)
{
	int i = 0;
	char kod_operacji, znak = ciag_znakow[i];

	if (znak == 'I') // IF
	{
		kod_operacji = IF;
		i += 2;
	}
	else if (znak == 'N') // N
	{
		kod_operacji = NEG;
		i++;
	}
	else if (znak == 'M')
	{
		if (ciag_znakow[i+1] == 'I') // MIN
			kod_operacji = MIN;
		else // MAX
			kod_operacji = MAX;
		i += 3;
	}
	else // + - / *
	{
		kod_operacji = znak;
		i++;
	}

	(*j) += i;

	return kod_operacji;
}

void przenies_znak_ze_stosu_pomocniczego(struct element** stos_pomocniczy, struct element** stos_docelowy, bool rodzaj_elementu)
{
	int zachowana_liczba = (*stos_pomocniczy)->liczba;
	char znak = pop(stos_pomocniczy, OPERATOR);
	push(OPERATOR, zachowana_liczba, znak, stos_docelowy); // TODO
}

void licznik_argumentow(struct element* stos_pomocniczy, int otwarte_nawiasy, int zabrane_przecinki)
{
	struct element* biezacy_element = stos_pomocniczy;
	while (biezacy_element != NULL)
	{
		if ((biezacy_element->dzialanie == MAX || biezacy_element->dzialanie == MIN || biezacy_element->dzialanie == IF) && otwarte_nawiasy)
		{
			biezacy_element->liczba++;
			biezacy_element->liczba = biezacy_element->liczba - zabrane_przecinki;
			otwarte_nawiasy--;
		}
		biezacy_element = biezacy_element->nastepny;
	}
}


/////// OBLICZANIE WYRAZEN ONP

int priorytet_opearcji(char kod_operacji)
{
	switch (kod_operacji) {
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	case IF:
		return 3;
	case NEG:
		return 4;
	case MIN:
	case MAX:
		return 5;
	default:
		return 0;
	}
}

void odwroc_stos(struct element* ptr, struct element** nowy_stos)
{
	while (ptr != NULL)
	{
		push(ptr->rodzaj_elementu, ptr->liczba, ptr->dzialanie, nowy_stos);
		ptr = ptr->nastepny;
	}
}

bool dzialanie_matematyczne(struct element** stos_pomocniczy, int znak)
{
	int operand2 = pop(stos_pomocniczy, LICZBA);
	int operand1 = pop(stos_pomocniczy, LICZBA);

	switch (znak) {
	case '+':
		push(LICZBA, operand1 + operand2, ' ', stos_pomocniczy);
		break;
	case '-':
		push(LICZBA, operand1 - operand2, ' ', stos_pomocniczy);
		break;
	case '*':
		push(LICZBA, operand1 * operand2, ' ', stos_pomocniczy);
		break;
	case '/': 
		if (operand2 == 0) {
			printf("ERROR\n");
			return false;
		}
		push(LICZBA, operand1 / operand2, ' ', stos_pomocniczy);
	}
	return true;
}

void negacja(struct element** stos_pomocniczy) // NEG
{
	int operand = pop(stos_pomocniczy, LICZBA);
	operand *= -1;
	push(LICZBA, operand, ' ', stos_pomocniczy);
}

void porownanie(struct element** stos_pomocniczy) // IF
{
	int operand1 = pop(stos_pomocniczy, LICZBA);
	int operand2 = pop(stos_pomocniczy, LICZBA);
	int operand3 = pop(stos_pomocniczy, LICZBA);
	if (operand3 > 0)
		push(LICZBA, operand2, ' ', stos_pomocniczy);
	else
		push(LICZBA, operand1, ' ', stos_pomocniczy);
}

void wartosc_maksymalna(struct element** stos_pomocniczy, int liczba_argumentow) // MAX
{
	int wartosc_maksymalna = INT_MIN, liczba;
	while (*stos_pomocniczy != NULL && (*stos_pomocniczy)->rodzaj_elementu != OPERATOR && liczba_argumentow)
	{
		liczba = pop(stos_pomocniczy, LICZBA);
		if (liczba > wartosc_maksymalna)
			wartosc_maksymalna = liczba;
		liczba_argumentow--;
	}
	push(LICZBA, wartosc_maksymalna, ' ', stos_pomocniczy);
}

void wartosc_minimalna(struct element** stos_pomocniczy, int liczba_argumentow) // MIN
{
	int wartosc_maksymalna = INT_MAX, liczba;
	while (*stos_pomocniczy != NULL && (*stos_pomocniczy)->rodzaj_elementu != OPERATOR && liczba_argumentow)
	{
		liczba = pop(stos_pomocniczy, LICZBA);
		if (liczba < wartosc_maksymalna)
			wartosc_maksymalna = liczba;
		liczba_argumentow--;
	}
	push(LICZBA, wartosc_maksymalna, ' ', stos_pomocniczy);
}

void oblicz_wyrazenie(struct element* ptr)
{
	struct element *stos = NULL, *stos_pomocniczy = NULL;
	int wynik = 0, counter = 0, liczba_argumentow;
	char znak;
	bool rodzaj_elementu;

	odwroc_stos(ptr, &stos);

	while (stos != NULL)
	{
		liczba_argumentow = INT_MAX;
		do {
			rodzaj_elementu = stos->rodzaj_elementu;

			if (rodzaj_elementu == LICZBA)
			{
				push(LICZBA, pop(&stos, LICZBA), ' ', &stos_pomocniczy);
			}
			else // OPERATOR
			{
				int liczba = stos->liczba;
				znak = pop(&stos, OPERATOR);
				push(OPERATOR, liczba, znak, &stos_pomocniczy);
				drukuj_stos_odwrocony(stos_pomocniczy);
				pop(&stos_pomocniczy, OPERATOR);
				
				switch (znak) {
				case '+':
				case '-':
				case '*':
				case '/':
					if (!dzialanie_matematyczne(&stos_pomocniczy, znak))
						return;
					break;
				case NEG:
					negacja(&stos_pomocniczy);
					break;
				case IF:
					porownanie(&stos_pomocniczy);
					break;
				case MAX:
					wartosc_maksymalna(&stos_pomocniczy, liczba+1); // liczba argumentow -> liczba przecinkow + 1
					break;
				case MIN:
					wartosc_minimalna(&stos_pomocniczy, liczba + 1);
				}
			}
		} while (rodzaj_elementu != OPERATOR);


	}
	wynik = pop(&stos_pomocniczy, LICZBA);

	printf("%d\n", wynik);
}


/////// KONWERSJA WYRAŻENIA NA ONP

void konwersja_ONP(char bufor[], struct element** stos_wynikowy, struct element** stos_pomocniczy)
{
	char znak;
	int liczba, otwarte_nawiasy = 0, uzyte_przecinki = 0;

	int j = 0;
	while (bufor[j] != '.')
	{
		if (bufor[j] >= '0' && bufor[j] <= '9') // wczytano liczbę
		{
			liczba = odczytaj_liczbe_z_ciagu(&bufor[j], &j);
			push(LICZBA, liczba, ' ', stos_wynikowy);
		}
		else if (bufor[j] == '(') // otwarto nawias
		{
			push(OPERATOR, 1, bufor[j], stos_pomocniczy);
			j++;
			otwarte_nawiasy++;
		}
		else if (bufor[j] == ')') // zamknieto nawiaas
		{
			while (*stos_pomocniczy != NULL && (*stos_pomocniczy)->dzialanie != '(')
			{
				if ((*stos_pomocniczy)->rodzaj_elementu == OPERATOR)
				{
					przenies_znak_ze_stosu_pomocniczego(stos_pomocniczy, stos_wynikowy, OPERATOR);
				}
				else // LICZBA
				{
					liczba = pop(stos_pomocniczy, LICZBA);
					push(LICZBA, liczba, ' ', stos_wynikowy);
				}
			}
			pop(stos_pomocniczy, OPERATOR); // usunięcie lewego nawiasu
			j++;
			otwarte_nawiasy--;

			// obsługa operacji funkcyjnych
			if (*stos_pomocniczy != NULL && priorytet_opearcji((*stos_pomocniczy)->dzialanie) >= OPERACJE_FUNKCYJNE)
			{
				(*stos_pomocniczy)->liczba -= uzyte_przecinki;
				uzyte_przecinki += (*stos_pomocniczy)->liczba;
				przenies_znak_ze_stosu_pomocniczego(stos_pomocniczy, stos_wynikowy, OPERATOR);
			}
		}
		else if (bufor[j] == ',') // podano kolejny argument
		{
			// obsługa operacji funkcyjnych
			licznik_argumentow(*stos_pomocniczy, otwarte_nawiasy, uzyte_przecinki);
			uzyte_przecinki = 0;

			while (*stos_pomocniczy != NULL && priorytet_opearcji((*stos_pomocniczy)->dzialanie) >= 1)
				przenies_znak_ze_stosu_pomocniczego(stos_pomocniczy, stos_wynikowy, OPERATOR);
			j++;
		}
		else if (bufor[j] == 'N' || bufor[j] == 'I' || bufor[j] == 'M') // wczytano operację funkcyjną
		{
			znak = odczytaj_operacje_z_ciagu(&bufor[j], &j);
			push(OPERATOR, 0, znak, stos_pomocniczy);

		}
		else // wczytano znak działania matematycznego
		{
			znak = odczytaj_operacje_z_ciagu(&bufor[j], &j);
			while (*stos_pomocniczy != NULL && priorytet_opearcji((*stos_pomocniczy)->dzialanie) >= priorytet_opearcji(znak))
			{
				if ((*stos_pomocniczy)->rodzaj_elementu == OPERATOR)
				{
					przenies_znak_ze_stosu_pomocniczego(stos_pomocniczy, stos_wynikowy, OPERATOR);
				}
				else // LICZBA
				{
					liczba = pop(stos_pomocniczy, LICZBA);
					push(LICZBA, liczba, ' ', stos_wynikowy);
				}
			}
			push(OPERATOR, 0, znak, stos_pomocniczy);
		}

		while (bufor[j] == ' ') // wczytano znak biały
			j++;
	}

	while (*stos_pomocniczy != NULL)
	{
		liczba = (*stos_pomocniczy)->liczba;
		push(OPERATOR, liczba, pop(stos_pomocniczy, OPERATOR), stos_wynikowy);
	}
}


int main()
{
	int n; // liczba formuł
	struct element* stos_wynikowy = NULL, *stos_pomocniczy = NULL;

	char bufor[8000];

	scanf_s("%d\n", &n);

	for (int i = 0; i < n; i++)
	{
		wyczysc_stos(&stos_wynikowy);
		wyczysc_stos(&stos_pomocniczy);

		fgets(bufor, sizeof(bufor), stdin); // wczytanie linii inputu

		konwersja_ONP(bufor, &stos_wynikowy, &stos_pomocniczy);

		drukuj_stos(stos_wynikowy);
		printf("\n");

		oblicz_wyrazenie(stos_wynikowy);
	}

	return 0;
}