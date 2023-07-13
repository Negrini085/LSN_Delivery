/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "CallPut.h"
#include "random.h"

using namespace std;

/********************************************************************************
*                                                                               *
*                                  IDEA CODICE                                  *
*                                                                               *  
*********************************************************************************
*                                                                               *
*  Valutiamo quanto valga il costo dell'opzione in caso di "Call Option" o      *
*  "Put Option" con due metodologie differenti:                                 *
*      1 -> utilizzo il metodo di sampling diretto sia un GBM(r, sigma^2)       *
*      2 -> utilizzo il metodo di campionamento discreto di S(t)                *
*                                                                               *
*  Il codice è organizzato come segue. In CallPut.h sono implementate quattro   *
*  classi. La prima è una classe madre virtuale contenente il metodo virtuale   *
*  puro Eval che consente di effettuare il campionamento di S(t). La prima      *
*  delle classi figlie è "Discreto", che effettua il campionamento discreto,    *
*  mentre la classe figlia "Diretto" calcola direttamente il prezzo dell'       *
*  opzione senza dover effettuare una partizione dell'intervallo temporale fra  *
*  data di scrittura del contratto e data di scadenza dello stesso. La quarta   *
*  classe è PutCall che con due metodi (Put oppure Call) consente di stimare    *
*  il prezzo delle due tipologie di opzioni. Tre funzioni consentono di         *
*  effettuare la stima call oppure put nel singolo blocco, per poi andare a     *
*  performare la media a blocchi.                                               *
*                                                                               *
********************************************************************************/
 
int main (int argc, char *argv[]){

   //Generatore numeri random
   Random rnd;
   
   //Lunghezza blocco e numero blocchi
   int n_blocchi = 100;
   int l_blocco = 1000;

   Init(rnd); //Inizializzo variabile per estrazione Random

   //Elementi per stime varie
   Discreto stima_1; Diretto stima_2; PutCall calcola;

   //Canale output
   vector<string> nomi_file {"CallDiscreto.dat", "PutDiscreto.dat"};

   cout << "Effettuo stima con metodo di campionamento discreto!" << endl;
   Stima(calcola, stima_1, l_blocco, rnd, n_blocchi, nomi_file);

   nomi_file = {"CallDiretto.dat", "PutDiretto.dat"};
   cout << "Effettuo stima con metodo di campionamento diretto!" << endl;
   Stima(calcola, stima_1, l_blocco, rnd, n_blocchi, nomi_file);


   rnd.SaveSeed(); 
   
   return 0;
}

/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/
