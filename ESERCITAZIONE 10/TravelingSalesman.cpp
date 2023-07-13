#include "TravelingSalesman.h"

//Funzione inizializzazione metodo random
void Init(Random& rnd, int rank) {

   int seed[4];
   int p1, p2, appo;

   ifstream Primes("../RANDOM_GEN/Primes");
   if (Primes.is_open()){
      for(int i=0; i<rank; i++) {
         Primes >> appo >> appo;
      }
      Primes >> p1 >> p2 ;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("../RANDOM_GEN/seed.in");
   string property;
   if (input.is_open()){
      while ( !input.eof() ){
         input >> property;
         if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            seed[3] += rank;
            rnd.SetRandom(seed,p1,p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

}