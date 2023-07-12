/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "random.h"

using namespace std;

//******FUNZIONI******//
double stima(int i, int n_somme, Random& rnd);
 
int main (int argc, char *argv[]){

   Random rnd;
   int seed[4];
   int p1, p2;
   ifstream Primes("../../RANDOM_GEN/Primes");
   if (Primes.is_open()){
      Primes >> p1 >> p2 ;
   } else cerr << "PROBLEM: Unable to open Primes" << endl;
   Primes.close();

   ifstream input("../../RANDOM_GEN/seed.in");
   string property;
   if (input.is_open()){
      while ( !input.eof() ){
         input >> property;
         if( property == "RANDOMSEED" ){
            input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
            rnd.SetRandom(seed,p1,p2);
         }
      }
      input.close();
   } else cerr << "PROBLEM: Unable to open seed.in" << endl;

    ofstream fileout; //Canale per output a file
   
   int n_stime = 10000; //Numero di stime per istogramma
   vector<int> n_somme {1, 2, 10, 100}; //Numero di somme per stima S_n
   vector<string> nome_file {"Uniforme.dat", "Esponenziale.dat", "Lorentziana.dat"}; //Vettore per nome files
   
   //Ciclo esterno che sceglie il metodo
   for(int i=0; i<3; i++) {

        //Apro comunicazione a file
        fileout.open(nome_file[i]);

        //Controllo su apertura file
        if(!fileout) {
            cerr << "Errore in apertura file: " << nome_file[i] << endl;
            cerr << "Blocco esecuzione programma!" << endl;
            return -1;
        }

        //Ciclo che mi consente di iterare sulla size delle somme
        for(int j=0; j<(int)n_somme.size(); j++) {
            //Ciclo che mi consente di fare la stima
            for(int k=0; k<n_stime; k++) {
                fileout << stima(i, n_somme[j], rnd) << "   ";
            }
            fileout << endl;
        }

        //Chiudo output a file
        fileout.close();
   }

   rnd.SaveSeed();
   return 0;
}

double stima(int i, int n_somme, Random& rnd) {

    double appo = 0;    //Variabile di appoggio
    
    //Utilizzo distribuzione uniforme
    if(i == 0) {
        for(int l=0; l<n_somme; l++) {
            appo = appo * l/(l+1) + rnd.Rannyu()/(l+1);
        }
        return appo;
    }

    //Utilizzo distribuzione esponenziale
    else if(i ==1) {
        for(int l=0; l<n_somme;l++) {
            appo = appo * l/(l+1) + rnd.Exponetial(1.)/(l+1);
        }
        return appo;
    }

    //Utilizzo distribuzione lorentiana
    for(int l=0; l<n_somme; l++) {
        appo = appo * l/(l+1) + rnd.Lorentian(1., 0)/(l+1);
    }

    return appo;
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
