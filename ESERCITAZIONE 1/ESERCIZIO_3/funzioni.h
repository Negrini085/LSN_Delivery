#ifndef __funzioni_h__
#define __funzioni_h__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "random.h"

using namespace std;


//Funzione che valuta la lunghezza di un vettore uscente da (0,0)
double lunghezza(double a, double b);
//Funzione che ritorna le coordinate y dei due estremi dell'ago (problema è invariante rispetto x)
vector<double> ago_random(Random& rnd, double dist_righe, double lungh_ago); 
//Funzione che ritorna la singola stima di pi greco
double stima_pi(Random& rnd, double dist_righe, double lungh_ago, double n_throws);
//Funzione per la stima di pi greco hit/miss
double pi_HitMiss(Random& rnd, int n_throws);


#endif //__funzioni_h__