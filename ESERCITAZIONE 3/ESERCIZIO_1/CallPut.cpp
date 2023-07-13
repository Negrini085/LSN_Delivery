/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include "CallPut.h"



//Funzione per inizializzare una variabile di tipo Random
void Init(Random& rnd) {

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



//Metodo Eval classe Diretto
double Diretto::Eval(Random& rnd) const {
   return m_S0 * exp((m_r - pow(m_vol, 2)/2) * m_T + m_vol * rnd.Gauss(0, m_T));
} 



//Metodo Eval classe Discreto
double Discreto::Eval(Random& rnd) const {

   double appo = 0; //Variabile d'appoggio
   appo = m_S0; //Step iniziale del GBM
   
   for(int i=1; i<= m_ndiv; i++) {
      appo = appo * exp((m_r - pow(m_vol, 2)/2) * m_T/m_ndiv + m_vol * rnd.Gauss(0, 1) * sqrt(m_T/m_ndiv));
   }

   return appo;
}



//Metodo Put classe PutCall
double PutCall::Put(Metodo& met, Random& rnd) { 
   return exp(-met.Get_r() * met.Get_T()) * max(0., m_K - met.Eval(rnd));
}



//Metodo Call classe PutCall
double PutCall::Call(Metodo& met, Random& rnd) { 
   return exp(-met.Get_r() * met.Get_T()) * max(0., met.Eval(rnd) - m_K);
}



//Funzione per stima Put nel singolo blocco
double stimaPut(PutCall& calcola, Metodo& met, int l_blocco, Random& rnd) {

   double media = 0;

   //Ciclo per calcolo nel singolo blocco
   for(int i=0; i<l_blocco; i++) {
      media = media * i/(i+1) + calcola.Put(met, rnd)/(i+1);
   }

   return media;
}



//Funzione per stima Call nel singolo blocco
double stimaCall(PutCall& calcola, Metodo& met, int l_blocco, Random& rnd) {

   double media = 0;

   //Ciclo per calcolo nel singolo blocco
   for(int i=0; i<l_blocco; i++) {
      media = media * i/(i+1) + calcola.Call(met, rnd)/(i+1);
   }

   return media;
}



//Funzione per stima Call/Put per un metodo di campionamento di S(t)
void Stima(PutCall& calcola, Metodo& met, int l_blocco, Random& rnd, int n_blocchi, vector<string>& nomi) {

    ofstream fileout;
    double appo = 0; double media = 0; double media2 = 0;

    for(int j=0; j<(int)nomi.size(); j++) {

        //Apro canale di output
        fileout.open(nomi[j]);
        if(!fileout) { cerr << "Errore in apertura file di output" << endl;}
        
        //Calcolo medie progressive ed errori all'aumentare del numero di blocchi (Put)
        for(int i=0; i<n_blocchi; i++){

            //Valuto stima nel blocco
            if(j==0) { appo = stimaCall(calcola, met, l_blocco, rnd); }
            else if(j==1) { appo = stimaPut(calcola, met, l_blocco, rnd); }

            //Calcolo medie e med2 progressive
            media = media * i/(i+1) + appo/(i+1); fileout << media << "   ";
            media2 = media2 * i/(i+1) + appo * appo/(i+1);

            //Assegno errore
            if(i==0) { fileout << 0 << endl; }
            else { fileout << sqrt((media2 - pow(media, 2))/i) << endl; }

        }

        //Chiudo
        fileout.close();

    }
    
}