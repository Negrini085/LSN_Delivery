/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "RandomWalk.h"
#include "random.h"

using namespace std;

/********************************************************************************
*                                                                               *
*                                  IDEA CODICE                                  *
*                                                                               *
*********************************************************************************
*                                                                               *
*   Lo scopo del seguente codice è valutare la distanza aspettata dall'origine  *
*   (punto di partenza del Random Walk) ad un certo tempo (numero di step) di   *
*   evoluzione del Random Walk. Questi valori verranno poi confrontati con      *
*   quanto teoricamente aspettato. In RandomWalk.cpp, RandomWalk.h è definita   *
*   la classe RandomWalk, che ha 3 data membri privati per contenere le         *
*   coordinate del walker, oltre ad un generatore di numeri casuali e ad altri  *
*   due parametri necessari. Due metodi distinti consentono di effettuare il    *
*   RW discreto oppure continuo. Nel main vengono eseguiti 10000 RW per tipo    *
*   ognuno lungo 100 step e vengono poi restituiti i valori di aspettazione     *
*   della distanza ad ogni tempo.                                               *
*                                                                               *
********************************************************************************/
 
int main (int argc, char *argv[]){

   RandomWalk cammino; //Variabile RandomWalk per simulazione

   int l_RWalk = 100; //Lunghezza del Random Walk
   int n_stime = 10000; //Numero di simulazioni di RandomWalk
   int n_blocchi = 100; //Numero di blocchi
   int l_blocchi = n_stime/n_blocchi; //Lunghezza blocco

   double appo = 0; //Variabile d'appoggio
   vector<double> medie; medie.assign(l_RWalk, 0); //Contenitore per medie nel blocco
   vector<double> med_prog; med_prog.assign(l_RWalk, 0); //Contenitore per medie progressive
   vector<double> med2_prog; med2_prog.assign(l_RWalk, 0); //Contenitore per medie2 progressive

   ofstream fileout;
   vector<string> nomi_file { "RWDiscreto.dat", "RWContinuo.dat" }; //Nomi file di output

   //Ciclo per lavorare con i due differenti metodi
   for(int i=0; i<(int)nomi_file.size(); i++) {

      //Lavoro sui blocchi
      for(int j=0; j<n_blocchi; j++) {

         //Effettuo stima nel singolo blocco
         for(int k=0; k<l_blocchi; k++) {
            
            //Eseguo il singolo random walk
            for(int h=0; h<l_RWalk; h++) {
               
               Step(i, cammino); //Eseguo lo step
               appo = cammino.Dist2();

               medie[h] = medie[h] * k/(k+1) + appo/(k+1);

            }

            //Riporto a zero il random walk
            cammino.SetX(0); cammino.SetY(0); cammino.SetZ(0);

         }

         //Aggiorno le medie progressive e med2 progressive
         for(int k=0; k<l_RWalk; k++) {
            med_prog[k] = med_prog[k] * j/(j+1) + sqrt(medie[k])/(j+1);
            med2_prog[k] = med2_prog[k] * j/(j+1) + medie[k]/(j+1);
         }

         //Riporto a zero tutte le celle del vettore medie
         medie.assign(l_RWalk, 0);

      }

      //Stampo a file i valori ottenuti
      fileout.open(nomi_file[i]);
      
      //Test correttezza apertura file
      if(!fileout) {
         cerr << "Problema in apertura file di output: " << nomi_file[i] << endl;
         cerr << "Blocco esecuzione del programma!" << endl;
         return -1;
      }

      for(int j=0; j<l_RWalk; j++) {
         fileout << med_prog[j] << "   ";
         fileout << sqrt((med2_prog[j] - pow(med_prog[j], 2))/l_RWalk) << endl;
      }
      fileout.close();

   }

   GraficoDiscreto(cammino, l_RWalk);
   cammino.SetX(0); cammino.SetY(0); cammino.SetZ(0);
   GraficoContinuo(cammino, l_RWalk);


   cammino.SaveSeed();
   
   return 0;
}

/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/
