/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "funzioni.h"
#include "random.h"

using namespace std;

/******************************************************************************
*                                                                             *
*                                IDEA CODICE                                  *
*                                                                             * 
*******************************************************************************
*                                                                             *
*   Il codice seguente consiste in un programma che consente di effettuare    *
*   il calcolo dell'integrale richiesto con quattro differenti metodologie.   *
*   In primo luogo si lavora con la distribuzione uniforme di probabilità,    *
*   andando a campionare uniformemente l'intervallo di integrazione. La       *
*   seconda stima consiste nel lavorare con una distribuzione di probabilità  *
*   pari allo sviluppo del coseno in zero bloccato al secondo ordine: tale    *
*   p(x) viene ottenuta mediante l'utilizzo del metodo Acc/Rej (che viene     *
*   testato a inizio codice per osservarne o meno la correttezza). Tale       *
*   distribuzione viene anche campionata con metodo ibrido. Infine per        *
*   lavorare con un metodo di sampling più efficace rispetto ad Acc/Rej       *
*   viene anche proposto il calcolo con l'importance sampling.                *
*                                                                             *
******************************************************************************/
 
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

   int n_throws = 100000; //Numero di tiri per il singolo integrale
   int N = 100; //Numero di blocchi

   double appo = 0; //Variabile d'appoggio
   double med_prog = 0; //Variabile media progressiva
   double med2_prog = 0; //Variabile media2 progressiva

   vector<string> nomi_file {"StimaUniforme.dat", "StimaDistr1AR.dat", "StimaDistr1Hyb.dat", "StimaDistr2Cum.dat"};


   /*****************************************************************
   *                   Testo varie distribuzioni                    *
   *****************************************************************/
   ofstream fileout;
   test(rnd);
      


   /****************************************************
   *          Effettuo stime degli integrali           *
   ****************************************************/
   for(int i = 0; i < (int)nomi_file.size(); i++) {
      
      //Canale di output per singolo metodo
      fileout.open(nomi_file[i]);

      cout << " Effettuo stima con il: " << i+1 << "-esimo metodo." << endl;

      //Medie progressive per singolo metodo
      for(int j=0; j<N; j++) {
         
         //Effettuo stima del singolo blocco
         appo = stima(n_throws, rnd, i);

         //Valuto media progressiva e med2 progressiva
         med_prog = med_prog * j/(j+1) + appo/(j+1);
         med2_prog = med2_prog * j/(j+1) + appo * appo/(j+1);

         fileout << med_prog << "   ";

         //Assegno errore
         if(j == 0) { fileout << 0 << endl; }
         else { fileout << sqrt((med2_prog - pow(med_prog, 2))/j) << endl; }

      }

      //Chiudo canale di output
      fileout.close();

      //Pongo a zero le variabili per media e media2 progressiva
      med_prog = 0; med2_prog = 0;
   }




   rnd.SaveSeed();
   return 0;
}

/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/
