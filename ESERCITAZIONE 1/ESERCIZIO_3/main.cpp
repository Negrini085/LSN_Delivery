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

#include "funzioni.h"
#include "random.h"

using namespace std;



/**************************************************************************
*                                                                         *
*                              IDEA CODICE                                *
*                                                                         *
****************************************************************************
*                                                                          *
*   Mi immagino di lavorare in una situazione con delle righe orizz.       *
*   parallele all'asse x. Un ago lanciato e' univocamente determinato      *
*   dalle coordinate dei suoi estremi: cio' che effettivamente e'          *
*   fondamentale per determinare la hit oppure la miss (per il sist.       *
*   considerato) e' l'odinata di ambo i punti. Dopo aver fissato la        *
*   lunhezza dell'ago e la separazione spaziale fra le righe si simula     *
*   il singolo lancio nel seguente modo:                                   *  
*                                                                          *
*   1 -> fisso a zero la coordinata x del primo estremo data l'invarianza  *
*        rispetto alle ascisse del sistema considerato                     *
*                                                                          *
*   2 -> scelgo random l'ordinata del primo estremo. Tale quantita' la     *
*        estraggo fra 0 e dist_righe in quanto mi immagino di avere una    *
*        riga ad y = 0 ed una seconda ad y = dist_righe (mi posso sempre   *
*        riportare ad una situazione simile mediante una traslazione       *
*                                                                          *
*   3 -> determino l'effettiva disposizione spaziale dell'ago. Per fare    *
*        questo estraggo randomicamente due valori appartenenti all'       *
*        intervalo [-1, 1]: essi saranno l'ascissa e l'ordinata di un      *
*        vettore di R^2. Se il modulo dello stesso e' maggiore di uno      *
*        straggo nuovamente le coordinare, altrimenti le rimodulo in modo  *
*        tale da avere un vettore unitario. Dopo aver moltiplicato tale    *
*        vettore per lungh_ago, determiniamo la coordinata y del secondo   *
*        estremo come: y2 = y1 + ordinata vettore rimodulato               *
*                                                                          *
*   4 -> Valuto se ho la hit oppure no testando se l'ordinata del secondo  *
*        estremo e' maggiore o uguale a dist_righe oppure minore o uguale  *
*        a zero.                                                           *
*                                                                          *
*   Questo processo viene ripetuto il numero di volte richiesto a blocco e *
*   per il numero richiesto di blocchi.                                    *
*                                                                          *
*                                                                          *
*   Viene inoltre fornita una stima del valore di pi_greco utilizzando     *
*   il metodo hit/miss lavorando con una circonferenza di raggio unitario  *
*   ed un quadrato di lati [-1, 1] x [-1, 1]                               *
*                                                                          *
***************************************************************************/


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



   double dist_righe = 2.; //Distanza fra righe adiacenti
   double lungh_ago = 0.5; //Lunghezza ago

   int n_throws = 100000; //Numero di lanci per stima (100000)
   int N = 100; //Numero di stime (100)

   double appo = 0;  //Variabile d'appoggio
   double med_prog = 0; //Media progressiva
   double med2_prog = 0;   //Media2 progressiva

   ofstream fileout;
   fileout.open("Buffon.dat");


   /**************************************************
   *       Ciclo per stima pi_greco con buffon       *
   **************************************************/
   for(int i=0; i<N; i++) {
      //Effettuo la singola stima di pi_greco
      appo = stima_pi(rnd, dist_righe, lungh_ago, n_throws);
      cout << appo << " ";

      //Calcolo valore medio e medio2 progressivo
      med_prog = med_prog * i/(i+1) + appo/(i+1);
      med2_prog = med2_prog * i/(i+1) + appo * appo/(i+1);

      fileout << med_prog << "   ";

      //Assegno valore dell'errore commesso
      if(i==0) { fileout << 0 << endl; }

      else {
         fileout << sqrt((med2_prog - pow(med_prog, 2))/i) << endl;
      }

      cout << "Finita " << i+1 << "-esima stima esperimento Buffon" << endl;

   }

   fileout.close();   //Chiudo comunicazione a file per buffon
   fileout.open("HitMiss.dat");

   cout << endl << endl;
   cout << "Effettuo la stima di pigreco con il metodo Hit/Miss" << endl;
   med_prog = 0; med2_prog = 0;

   /**************************************************
   *        Ciclo per stima pi_greco Hit/Miss        *
   **************************************************/
   for(int i=0; i<N; i++) {
      //Effettuo la singola stima di pi_greco
      appo = pi_HitMiss(rnd, n_throws);

      //Calcolo valore medio e medio2 progressivo
      med_prog = med_prog * i/(i+1) + appo/(i+1);
      med2_prog = med2_prog * i/(i+1) + appo * appo/(i+1);

      fileout << med_prog << "   ";

      //Assegno valore dell'errore commesso
      if(i==0) { fileout << 0 << endl; }

      else {
         fileout << sqrt((med2_prog - pow(med_prog, 2))/i) << endl;
      }

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
