/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#ifndef __funzioni_h__
#define __funzioni_h__

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "random.h"

using namespace std;

//Funzione per estrarre una x distribuita come: p(x) = (6/5)*[1-(x^2)/2] (con A/R)
double Distr1_AR(Random& rnd);
//Funzione per estrarre una x distribuita come: p(x) = (6/5)*[1-(x^2)/2] (con metodo ibrido)
double Distr1_Hyb(Random& rnd);
//Funzione per estrarre una x distribuita come: p(x) = 2 * (1 - x) (con metodo cumulativa)
double Distr2_Cum(Random& rnd);

//Funzione per test delle distribuzioni di probabilità
void test(Random& rnd);

//Funzione per la singola stima dell'integrale (distribuzione uniforme)
double stima_unif(int n_throws, Random& rnd);
//Funzione per la singola stima dell'integrale (distribuzione p(x) = (6/5)*[1-(x^2)/2] A/R)
double stima_distr1AR(int n_throws, Random& rnd);
//Funzione per la singola stima dell'integrale (distribuzione p(x) = (6/5)*[1-(x^2)/2] con metodo ibrido)
double stima_distr1Hyb(int n_throws, Random& rnd);
//Funzione per la singola stima dell'integrale (distribuzione p(x) = 2 * (1 - x) cumulativa)
double stima_distr2Cum(int n_throws, Random& rnd);

//Funzione per stima integrale
double stima(int n_throws, Random& rnd, int& i);

#endif //__funzioni_h__