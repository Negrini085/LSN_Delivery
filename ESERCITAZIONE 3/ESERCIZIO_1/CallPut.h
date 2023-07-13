/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

#include "random.h"

using namespace std;



//Funzione per inizializzare una variabile di tipo Random
void Init(Random& rnd);



//Classe madre virtuale per metodo di cambionamento della Geometric Brownian motion
class Metodo {

   public:
   //Costruttore di default
   Metodo() { m_T = 1.; m_S0 = 100; m_r = 0.1; m_vol = 0.25; }
   //Costruttore con input
   Metodo(double T, double S0, double r, double vol) { m_T = T; m_S0 = S0; m_r = r; m_vol = vol; }
   //Distruttore virtuale
   virtual ~Metodo() {;}

   //Metodi Set
   void Set_T(double T) { m_T = T; }
   void Set_S0(double S0) { m_S0 = S0; }
   void Set_r(double r) { m_r = r; }
   void Set_vol(double vol) { m_vol = vol; }

   //Metodi get
   double Get_T() { return m_T; }
   double Get_S0() { return m_S0; }
   double Get_r() { return m_r; }
   double Get_vol() { return m_vol; }

   //Metodo virtuale
   virtual double Eval(Random& rnd) const = 0;

   protected:
   //Variabili per il campionamento diretto della Geometric Brownian Motion
   double m_T, m_S0, m_r, m_vol;

};



//Classe per il campionamento diretto
class Diretto : public Metodo {

   public:
   //Costruttore di default (con valori forniti sul jupyter notebook)
   Diretto() : Metodo() {;}
   //Costruttore
   Diretto(double T, double S0, double r, double vol) : Metodo(T, S0, r, vol) {;}
   //Distruttore
   ~Diretto() override {;}

   //Metodo Eval
   double Eval(Random& rnd) const override;

};



//Classe per il campionamento discreto
class Discreto : public Metodo {

   public:
   //Costruttore di default (con valori forniti sul jupyter notebook)
   Discreto() : Metodo() {m_ndiv = 100;}
   //Costruttore
   Discreto(double T, double S0, double r, double vol, int ndiv) : Metodo(T, S0, r, vol) { m_ndiv = ndiv; }
   //Distruttore
   ~Discreto() override {;}

   //Metodi Set
   void Set_ndiv(double ndiv) { m_ndiv = ndiv; }

   //Metodi Get
   double Get_ndiv() { return m_ndiv; }


   //Metodo Eval
   double Eval(Random& rnd) const override;


   private:
   //Numero di intervalli in cui divido per campionamento discreto
   int m_ndiv;

};



//Classe Put/Call 
class PutCall {
    public:
    //Costruttore di default
    PutCall() { m_K = 100; }
    //Costruttore con input
    PutCall(double K) { m_K = K; }
    //Distruttore
    ~PutCall() {;}

    //Metodi Get
    double GetK() { return m_K; }

    //Metodi Set
    void SetK(double K) { m_K = K; }

    //Metodi Put e Call
    double Put(Metodo& met, Random& rnd);
    double Call(Metodo& met, Random& rnd);


    private:
    double m_K;
};



//Funzione per stima Put nel singolo blocco
double stimaPut(PutCall& calcola, Metodo& met, int l_blocco, Random& rnd);



//Funzione per stima Call nel singolo blocco
double stimaCall(PutCall& calcola, Metodo& met, int l_blocco, Random& rnd);



//Funzione per stima Call/Put per un metodo di campionamento di S(t)
void Stima(PutCall& calcola, Metodo& met, int l_blocco, Random& rnd, int n_blocchi, vector<string>& nomi);