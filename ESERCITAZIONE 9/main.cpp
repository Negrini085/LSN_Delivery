#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#include "TravelingSalesman.h"
#include "random.h"

using namespace std;


int main(){

   //Generatore di numeri casuali
   Random rnd;
   Init(rnd);

   //Valori di input
   ifstream filein;
   int ncity, forma, ncamm, ngen;

   filein.open("input.in");
   if(!filein) {
      cout << "Errore in apertura file di input." << endl;
      cout << "Esco!!" << endl;
      return 0;
   }
   filein >> ncity >> forma >> ncamm >> ngen;
   filein.close();

   TravelingSalesman campione(rnd, ncity, forma, ncamm);

   campione.GA(ngen, rnd);
   campione.StampaBest();

   rnd.SaveSeed();

return 0;
}

