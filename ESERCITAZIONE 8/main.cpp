/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "random.h"
#include "classi.h"

using namespace std;


/*************************************************************************************
*                                                                                    *
*                                     IDEA CODICE                                    *
*                                                                                    *
**************************************************************************************
*                                                                                    * 
*   Lo scopo del seguente codice è effettuare un ottimizzazione per SA in modo       *
*   tale da individuare una buona approssimazione per il ground state di un          *
*   problema quantistico. Per fare questo in classi.h sono implementate 3 classi     *
*   differenti.                                                                      *
*                                                                                    *
*   La prima classe è "var_Mod2" e consente di campionare via algoritmo di           * 
*   Metropolis la distribuzione di probabilità ricercata. Essa ha come data          *
*   membri protetti mu, sigma, delta ed un generatore di numeri casuali. Mu e        *
*   sigma sono i due parametri da cui dipende la nostra modellizzazione del          *
*   ground state. Delta consente di campionare uniformemente un intervallo           *
*   centrato in x (x - delta/2, x + delta/2) quando viene proposta una move nel      *
*   campionamento. Il generatore di numeri casuali serve per proporre unif           *
*   le moves e per accettare o no la move stessa. Il metodo che effettivamente       *
*   campiona è: int Move(double& x). L'intero ritornato è 1 se la mossa prop.        *
*   è accettata (in questo caso cambia ovviamente anche x), oppure 0 nella           *
*   casistica opposta. Questo consente di andare a calcolare l'acceptance            *
*   rate.                                                                            *
*                                                                                    *
*   La seconda classe è "asp_H" e consente di effettuare il calcolo del singolo      *
*   integrale. Essa ha data membri protetti il numero di blocchi e i punti per       *
*   blocco, oltre a due variabili per immagazzinare il valore dell'integrale e       *
*   l'errore ad esso associato. Il metodo che effettivamente performa l'             *
*   integrazione è: void Integrale(var_Mod2& val, int stampa). Il secondo            *
*   parametro serve per decidere se stampare a file/terminale oppure no              *
*   i valori dei singoli blocchi e i rispettivi acceptance rate, mentre il           *
*   primo per campionare la p(x).                                                    *
*                                                                                    *
*   La terza classe è "SimAnnealing" e consente di effettuare la vera e propria      *
*   ottimizzazione. Essa ha come data membri protetti la temperatura iniziale,       *
*   quella a cui voglio far finire la termalizzazione, una variabile che             *
*   contenga beta, due variabili che consentono di contenere il vecchio              *
*   integrale e quello nuovo ed un generatore di numeri casuali per proporre         *
*   i cambiamenti dei parametri mu e sigma. Il Simulated Annealing è performato      *
*   da void SA(asp_H& calcola, var_Mod2& campiona), ossia un metodo che necessita    *
*   di un elemento di tipo asp_H per calcolare i vari integrali e di uno di tipo     *
*   var_Mod2 per campionare la distribuzione di probabilità.                         *
*                                                                                    *
*************************************************************************************/



int main (int argc, char *argv[]){

    double delta, x;
    double mu, sigma;
    double Tin, Tfin, delta1;
    int nblk, dimblk, camp, appo;

    ofstream fileout;
    ifstream ReadInput;
    ReadInput.open("input.in");

    //Controllo corretta apertura del file
    if(!ReadInput) {
        cerr << "Errore in apertura file di input" << endl;
        cerr << "Termino esecuzione del programma" << endl;
        return 0;
    }

    ReadInput >> delta;
    ReadInput >> nblk;
    ReadInput >> dimblk;
    ReadInput >> mu;
    ReadInput >> sigma;
    ReadInput >> Tin;
    ReadInput >> Tfin;
    ReadInput >> delta1;
    ReadInput >> camp;


    var_Mod2 campiona(mu, sigma, delta);
    asp_H calcola(nblk, dimblk);
    SimAnnealing ottimizzazione(Tin, Tfin, delta1);

    
    /**********************************************
    *             Simulated Annealing             *
    **********************************************/
    ottimizzazione.SA(calcola, campiona);
    cout << endl << endl;


    /**********************************************
    *         Integrale (parametri ott.)          *
    **********************************************/
    calcola.Integrale(campiona, 1);
    

    /**********************************************
    *       Campionamento (parametri ott.)        *
    **********************************************/
    fileout.open("Campionamento.dat");
    //Controllo l'apertura del file
    if(!fileout) {
        cout << "Errore in apertura file di ouput: Campionamento.dat" << endl;
        cout << "Blocco l'esecuzione del programma: ciao!!" << endl;
        return 0;
    }

    x = 0; appo = 0;
    fileout << x << endl;
    for(int i=0; i<camp; i++){
        appo += campiona.Move(x);
        fileout << x << endl;
    }

    cout << endl << "Effettuato campionamento modulo quadro" << endl;
    cout << "Acceptance rate: " << (double)appo/camp * 100 << "%" << endl;

    fileout.close();
    

    /**********************************************
    *        Calcoli integrali - grafico          *
    **********************************************/
    cout << endl << endl << "Eseguo calcolo per grafico energia in funzione di mu e sigma" << endl;
    fileout.open("Grafico.dat");
    //Controllo l'apertura del file
    if(!fileout) {
        cout << "Errore in apertura file di ouput: Grafico.dat" << endl;
        cout << "Blocco l'esecuzione del programma: ciao!!" << endl;
        return 0;
    }

    for(int i=0; i<=10; i++){
        
        mu = 0.5 + 0.1 * i;
        
        for(int j=0; j<=10; j++){
            sigma = 0.5 + 0.1 * j;

            campiona.Set_mu(mu);
            campiona.Set_sigma(sigma);

            calcola.Integrale(campiona, 0);
            fileout << calcola.Get_integrale() << " ";
        }
        fileout << endl;
    }

    fileout.close();
    

    return 0;
}





/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/


/**********************************************
*                 Parametri                   *
***********************************************
*     sigma = 1                               *
*     mu = 1                                  *
*     delta = 7 (accept rate circa 50%)       *
**********************************************/

/*
    campiona.Set_mu(-0.886557);
    campiona.Set_sigma(0.482185);

    calcola.Set_nblk(100);
    calcola.Set_dimblk(10000);
    calcola.Integrale(campiona, 1);

    cout << endl << calcola.Get_integrale() << endl;
*/