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



   /*****************************************************
   *        CALCOLO MEDIE E VARIANZE PROGRESSIVE        *
   *****************************************************/


   //Numero di valori che vengono generati e numero di blocchi
   int n_gen  = 10000;
   int n_blocchi = 100;

   //Lunghezza del singolo blocco
   int lungh = n_gen/n_blocchi;

   //Variabili d'appoggio
   double med = 0;   //Media fatta nel singolo blocco
   double var = 0;   //Varianza fatta nel singolo blocco
   double estratto = 0; //Valore estratto randomicamente

   double med_prog = 0;  //Valore medio progressivo
   double var_prog = 0;  //Valore varianza progressiva
   double med2_prog = 0; //Valore media2 progressivo
   double var2_prog = 0; //Valore varianza2 progressiva
   double stddev_prog = 0; //Valore deviazione standard progressiva

   //File dove vado ad immagazzinare i valori
   ofstream fileout, file_out;
   fileout.open("MedieProg.dat");
   file_out.open("VarProg.dat");


   for(int i=0; i < n_blocchi; i++) {
      for(int j=0; j < lungh; j++) {
         estratto = rnd.Rannyu();
         med = med * j/(j+1) + estratto/(j+1); //Determino valore medio del blocco
         var = var * j/(j+1) + pow(estratto - 0.5, 2)/(j+1); //Determino varianza del singolo blocco
      }
      med_prog = med_prog * i/(i+1) + med/(i+1); //Valuto media progressiva
      var_prog = var_prog * i/(i+1) + var/(i+1); //Valuto varianza progressiva
      fileout << med_prog << "   "; //Stampo a file
      file_out << var_prog << "  "; //Stampo a file

      med2_prog = med2_prog * i/(i+1) + med*med/(i+1); //Valuto media2 progressiva
      var2_prog = var2_prog * i/(i+1) + var*var/(i+1); //Valuto varianza2 progressiva



      //Se ho solo una misura non è definita la varianza del campione
      if(i == 0) { fileout << 0 << endl;  file_out << 0 << endl; }
      
      //Tratto gli altri casi
      else {
         stddev_prog = sqrt((med2_prog - pow(med_prog, 2))/(i));
         fileout << stddev_prog << endl;
         stddev_prog = sqrt((var2_prog - pow(var_prog, 2))/i);
         file_out << stddev_prog << endl;
      }

      //Pongo nuovamente a zero med e var
      med = 0; var = 0;

   }

   //Chiudo file di output per medie e varianze progressive
   fileout.close();
   file_out.close();




   /*************************************
   *        TEST DEL CHI QUADRO         *
   *************************************/

   int M = 100; //Numero di intervalli in cui separo [0,1]
   int n = 10000; //Numero di estrazioni per ciclo

   int indice = 0;   //mi serve per capire quale degli ni andare ad aumentare di una unità
   double chi2 = 0;  //variabile per immagazzinare chi2

   //Apro comunicazione a file
   fileout.open("Chi2.dat");

   //Contenitore estrazioni per intervallino
   vector<int> n_eventi(M, 0);

   //Ciclo esterno per eseguire il test 100 volte
   for(int i=0; i < M; i++) {

      //Ciclo interno che fa n estrazioni a volta
      for(int j=0; j<n; j++) {
         estratto = rnd.Rannyu();
         indice = (int)floor(estratto * 100);

         //Aumento di una unità in numero di eventi avvenuti nell'indicesimo intervallo
         n_eventi[indice] += 1;

      }


      //Calcolo effettivo di chi_quadro
      for(int j=0; j<M; j++) {
         chi2 += pow((n_eventi[j] - n/M),2)/(n/M);
      }

      //Stampo valore a file e pongo a zero chi2 e contenitore numeri
      fileout << chi2 << endl;
      chi2 = 0;
      fill(n_eventi.begin(), n_eventi.end(), 0);
   }

   fileout.close();

   rnd.SaveSeed();
   return 0;
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
