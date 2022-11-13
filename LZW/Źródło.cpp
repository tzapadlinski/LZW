#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;


bool LZW_porownaj(vector<uint8_t> a, vector<uint8_t> b)
{
	if (a.size() != b.size())
		return 0;
	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
		{
			return 0;
		}
	}
	return 1;
}

void LZW(uint8_t* wejscie, int n)
{
	vector<uint8_t> slownik[239];		//255-16 = 239
	int ileWslowniku = 0;
	uint8_t pierwszy{};
	uint8_t nastepny{};
	uint8_t maskaMSB{ 0b11110000 };
	uint8_t maskaLSB{ 0b00001111 };
	vector<uint8_t> sprawdzany;
	bool h = 0;
	int i = -1;
	while (i < n)
	{
		sprawdzany.clear();
		if (h)
		{
			pierwszy = wejscie[i] & maskaLSB;
			h = 0;
		}
		else
		{
			if (i < n - 1)
			{
				i++;
				pierwszy = wejscie[i] & maskaMSB;
				pierwszy = pierwszy >> 4;
				h = 1;
			}
			else
			{
				i++;
				break;
			}
		}
		sprawdzany.push_back(pierwszy);
		int numer = -1;
		bool koniec = 1;
		do
		{
			if (h)
			{
				nastepny = wejscie[i] & maskaLSB;
				h = 0;
			}
			else
			{
				if (i < n - 1)
				{
					i++;
					nastepny = wejscie[i] & maskaMSB;
					nastepny = nastepny >> 4;
					h = 1;
				}
				else
				{
					i++;
					break;
				}

			}
			sprawdzany.push_back(nastepny);
			for (int s = 0; s < ileWslowniku; s++)
			{
				if (LZW_porownaj(sprawdzany, slownik[s]))
				{
					numer = s;
					koniec = 0;
					break;
				}
				koniec = 1;
			}
		} while (!koniec);
		if (numer == -1)
		{
			cout << (int)pierwszy << " ";
		}
		else
		{
			cout << (int)numer + 16 << " ";
		}
		if (ileWslowniku < 239)
		{
			slownik[ileWslowniku] = sprawdzany;
			ileWslowniku++;
		}
		if (h)
		{
			i--;			//dziêki temu w nastêpnym cyklu poprzednie czytane jest jako ostatnie nastêpne lepiej NIE RUSZAÆ!
		}
		h = !h;					//odwraca h po skoñczeniu, wtedy nastêpny cykl zacznie siê od tego, na którym siê skoñczy³o
	}
}

void czytajLZW(uint8_t* wyjscie, int n, uint8_t* wejscie, int m)
{
	vector<uint8_t> slownik[65520];		//65536-16 = 65520
	vector<uint8_t> doSlownika;
	int ileWslowniku = 0;
	uint8_t maskaMSB{ 0b11110000 };
	uint8_t maskaLSB{ 0b00001111 };
	uint16_t maskaWE{ 0b1111111111110000 };
	bool h = 1;
	int idWy = 0;
	doSlownika.push_back((uint8_t)wejscie[0]);
	wyjscie[0] = (uint8_t)(wejscie[0] << 4);
	for (int i = 1; i < m; i++)
	{
		if ((int)(wejscie[i] & maskaWE) == 0)
		{
			doSlownika.push_back((uint8_t)wejscie[i]);
			if (h)
			{
				wyjscie[idWy] = wyjscie[idWy] | (uint8_t)wejscie[i];
				h = 0;
				idWy++;
			}
			else
			{
				wyjscie[idWy] = (uint8_t)(wejscie[i] << 4);
				h = 1;
			}
			slownik[ileWslowniku] = doSlownika;
			ileWslowniku++;
			doSlownika.clear();
			doSlownika.push_back((uint8_t)wejscie[i]);
		}
		else
		{
			if (wejscie[i] - 16 >= ileWslowniku)
			{										//nie ma w slowniku
				doSlownika.push_back(doSlownika[0]);
				for (int j = 0; j < doSlownika.size(); j++)
				{
					if (h)
					{
						wyjscie[idWy] = wyjscie[idWy] | (uint8_t)doSlownika[j];
						h = 0;
						idWy++;
					}
					else
					{
						wyjscie[idWy] = (uint8_t)(doSlownika[j] << 4);
						h = 1;
					}
				}
				slownik[ileWslowniku] = doSlownika;
				ileWslowniku++;
			}
			else
			{										//jest w slowniku
				vector<uint8_t> temp = slownik[wejscie[i] - 16];
				doSlownika.push_back(temp[0]);			//do s³ownika zostanie wpisany pierwszy element
				for (int j = 0; j < temp.size(); j++)
				{
					if (h)
					{
						wyjscie[idWy] = wyjscie[idWy] | (uint8_t)temp[j];
						h = 0;
						idWy++;
					}
					else
					{
						wyjscie[idWy] = (uint8_t)(temp[j] << 4);
						h = 1;
					}
				}
				slownik[ileWslowniku] = doSlownika;
				ileWslowniku++;
				doSlownika.clear();
				doSlownika = temp;
			}
		}
	}
}


int main()
{
	srand(time(NULL));
	int n = 30;
	int m = n/2;
	uint8_t* tb = new uint8_t[n];
	for (int i = 0; i < n; i++)
	{
		tb[i] = rand() % 255;
		cout << (int)tb[i] << " ";
	}
	cout << endl;
	uint8_t* wy = new uint8_t[m];
	for (int i = 0; i < m; i++)
		cout << (int)wy[i] << " ";
	cout << endl;
	uint8_t* wy2 = new uint8_t[n];
	czytajLZW(wy2, n, wy, m);
	for (int i = 0; i < n; i++)
		cout << (int)wy2[i] << " ";
	return 0;
}