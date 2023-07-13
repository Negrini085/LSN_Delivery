/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include "RandomWalk.h"

//Funzione per inizializzazione di elemento di tipo Random
void init(Random& rnd) {

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

}



//Metodo per step discreto
void RandomWalk::StepDiscr() {
        
    //Scelgo quale coordinata (x,y o z) aggiornare
    m_appo = m_rnd.Rannyu();

    if(m_appo < 1./3.) {

        //Scelgo se aumentare o diminuire di una unità coordinata x
        m_appo = m_rnd.Rannyu();

        if(m_appo < 0.5) { m_x += m_a; }
        else { m_x -= m_a; }
    }

    else if(m_appo < 2./3. && m_appo > 1./3.) {

        //Scelgo se aumentare o diminuire di una unità coordinata y
        m_appo = m_rnd.Rannyu();

        if(m_appo < 0.5) { m_y += m_a; }
        else { m_y -= m_a; }
    }

    else if(m_appo > 2./3.) {

        //Scelgo se aumentare o diminuire di una unità coordinata z
        m_appo = m_rnd.Rannyu();

        if(m_appo < 0.5) { m_z += m_a; }
        else { m_z -= m_a; }
    }

}



//Metodo per step continuo
void RandomWalk::StepCont() {

   //Scelgo uniformemente la direzione del prossimo step
   double theta = m_rnd.Rannyu(0, M_PI);
   double phi = m_rnd.Rannyu(0, 2 * M_PI);

   //Aggiorno le varie posizioni
   m_x += m_a * sin(theta) * cos(phi);
   m_y += m_a * sin(theta) * sin(phi);
   m_z += m_a * cos(theta);

}



//Funzione per decidere quale tipo di step (discreto oppure continuo) fare
void Step(int i, RandomWalk& cammino) {

   //In primo caso faccio step discreto
   if(i == 0) { cammino.StepDiscr(); }

   //In secondo caso faccio step continuo
   else { cammino.StepCont(); }

}



//Grafico RandomWalk discreto
void GraficoDiscreto(RandomWalk& cammino, int l_RWalk) {

   ofstream fileout; fileout.open("GraficoDiscreto.dat");
   if(!fileout) { cerr << "Errore in apertura file output grafico" << endl; }

   //Stampo a file posizione iniziale(che è l'origine)
   fileout << cammino.GetX() << "   " << cammino.GetY() << "   " << cammino.GetZ() << endl;

   for(int i=0; i<l_RWalk; i++) {
      cammino.StepDiscr(); //Faccio step
      fileout << cammino.GetX() << "   " << cammino.GetY() << "   " << cammino.GetZ() << endl; //Stampo
   }

   fileout.close();

}



//Grafico RandomWalk continuo
void GraficoContinuo(RandomWalk& cammino, int l_RWalk) {

   ofstream fileout; fileout.open("GraficoContinuo.dat");
   if(!fileout) { cerr << "Errore in apertura file output grafico" << endl; }

   //Stampo a file posizione iniziale(che è l'origine)
   fileout << cammino.GetX() << "   " << cammino.GetY() << "   " << cammino.GetZ() << endl;

   for(int i=0; i<l_RWalk; i++) {
      cammino.StepCont(); //Faccio step
      fileout << cammino.GetX() << "   " << cammino.GetY() << "   " << cammino.GetZ() << endl; //Stampo
   }

   fileout.close();
}