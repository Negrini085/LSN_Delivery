#ifndef __classi_h__
#define __classi_h__

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "random.h"

using namespace std;

//Funzione per inizializzazione di elemento di tipo Random
void init(Random& rnd);
//Funzione per calcolo del modulo quadro
double mod2(double x, double sigma, double mu);
//Integranda
double integranda(double x, double sigma, double mu);




//Classe per modulo quadro funzione d'onda
class var_Mod2{

    public:
    //Costruttore di default
    var_Mod2(double mu, double sigma, double delta) { init(rnd); m_mu = mu; m_sigma = sigma; m_delta = delta; }
    //Distruttore
    ~var_Mod2() {;}


    //Metodi Get
    double Get_mu() { return m_mu; }
    double Get_sigma() { return m_sigma; }
    double Get_delta() { return m_delta; }

    //Metodi Set
    void Set_mu(double mu) { m_mu = mu; }
    void Set_sigma(double sigma) { m_sigma = sigma; }
    void Set_delta(double delta) { m_delta = delta; }

    
    //Metodo move per campionare il modulo quadro della funzione d'onda
    int Move(double& x) {

        /*********************************************************************************
        *    Noto che il parametro x è la vecchia posizione, per determinare la nuova    *
        *    devo proporre una move lavorando con il parametro delta.                    *
        *********************************************************************************/

        double x_new = 0;    //Nuova posizione
        double prob = 0;     //Probabilità di movimento

        x_new = x + (rnd.Rannyu() - 0.5) * m_delta;    //Determino nuova posizione
        prob = mod2(x_new, m_sigma, m_mu)/mod2(x, m_sigma, m_mu);

        if(rnd.Rannyu() < prob){ //Effettuo il passo
            x = x_new; 
            return 1; 
        } 

        return 0; //Non effettuo il passo
    }

    protected:
    Random rnd;
    double m_mu, m_sigma, m_delta;

};





class asp_H{

    public:
    //Costruttore di default
    asp_H() { n_blk = 0; dim_blk = 0; m_int = 0; m_error = 0; }
    //Costruttore (parametri dimensione blocco e numero blocchi)
    asp_H(double num, double dim) { n_blk = num; dim_blk = dim; m_int = 0; m_error = 0; }
    //Distruttore
    ~asp_H() {;}

    //Metodi Get
    int Get_nblk() { return n_blk; }
    int Get_dimblk() { return dim_blk; }
    double Get_integrale() { return m_int; }
    double Get_errore() { return m_error; }

    //Metodi Set
    void Set_nblk(int blk) { n_blk = blk; }
    void Set_dimblk(int dim) { dim_blk = dim; }
    void Set_integrale(double inte) { m_int = inte;}
    void Set_errore(double err) { m_error = err;}

    //Metodo per calcolo dell'integrale
    void Integrale(var_Mod2& val, int stampa) {

        double x = 0;
        double appo = 0;
        double appo2 = 0;
        int acce = 0;

        ofstream fileout;
        if(stampa == 1){
            fileout.open("Integrale.dat");
        }

        m_int = 0; m_error = 0;

        //Ciclo esterno per i blocchi
        for(int i=0; i<n_blk; i++) {
            
            //Riporto a zero variabili riguardanti il singolo blocco
            appo = 0;
            acce = 0;

            if(stampa == 1) { cout << endl << "--------------------------------------------------------" << endl; }

            //Ciclo interno per il singolo blocco
            for(int j=0; j<dim_blk; j++) {
                appo  = appo * j/(j+1) + integranda(x, val.Get_sigma(), val.Get_mu())/(j+1);
                acce += val.Move(x);
            }

            m_int = m_int * i/(i+1) + appo/(i+1);   //Stima integrale post i-esimo blocco
            appo2 = appo2 * i/(i+1) + appo * appo/(i+1);    //Valore per errore

            //Messaggio a terminale
            if(stampa == 1) {
                cout << endl << endl << "Calcolata stima " << i+1 << "-esimo blocco" << endl;
                cout << "Accettazione: " << (double)acce/dim_blk * 100 << " %" << endl;

                if(i==0) { m_error = 0; }
                else { m_error = sqrt((appo2 - pow(m_int, 2))/(i)); }

                fileout << m_int << "   " << m_error << endl;
            }
            
            
        }

        if(n_blk != 1) { m_error = sqrt((appo2 - pow(m_int, 2))/(n_blk - 1)); } //Associo errore alla stima
        
        if(stampa == 1) {
            cout << endl << "--------------------------------------------------------" << endl;
            cout << endl << endl << "Effettuata stima dell' integrale con: sigma = " << val.Get_sigma() << "   mu = " << val.Get_mu() << endl;
            cout << "Valore integrale: " << m_int << endl;
            cout << "Errore integrale: " << m_error << endl << endl << endl;
            fileout.close();
        }

    }



    protected:
    int n_blk, dim_blk;
    double m_int, m_error;

};





class SimAnnealing{

    public:
    //Costruttore di default
    SimAnnealing() { 
        T_in = 10; T_fin = 0.000001; m_beta = 0; 
        m_new = 0; m_old = 0; 
        init(rand); m_delta = 0.1;
    }
    //Costruttore (parametri dimensione blocco e numero blocchi)
    SimAnnealing(double tin, double tfin, double delta) { 
        T_in = tin; T_fin = tfin; m_beta = 0; 
        m_new = 0; m_old = 0; 
        init(rand); m_delta = delta;
    }
    //Distruttore
    ~SimAnnealing() {;}

    //Metodi Get
    double GetTin(){ return T_in; }
    double GetTfin(){ return T_fin; }

    //Metodi Set
    void SetTin(double tin) { T_in = tin; }
    void SetTfin(double tfin) { T_in = tfin; }

    void SA(asp_H& calcola, var_Mod2& campiona) {

        double p = 0;   //Probabilità di accettare la mossa
        int acce = 0;   //Numero di mosse accettato
        double T = T_in;    //Temperatura di partenza SA
        double appo, pippo; //Variabili di appoggio per le mosse

        ofstream fileout;   //Canale di output
        ofstream file_out;   //Canale di output
        fileout.open("SimAnnealing.dat");
        file_out.open("Parametri.dat");

        //Calcolo l'integrale nelle condizioni iniziali (mu = 1, sigma = 1)
        calcola.Integrale(campiona, 0);
        m_old = calcola.Get_integrale();
        fileout << m_old << "   " << calcola.Get_errore() << endl;
        file_out << campiona.Get_mu() << "   " << campiona.Get_sigma() << endl;

        while(T >= T_fin){

            acce = 0;   //Re-setto acce a zero
            m_beta = 1/T;   //Calcolo parametro beta
            
            //Voglio accettare almeno 5 mosse
            while(acce <= 5){

                //Salvo momentaneamente i valori di mu/sigma 
                //(vanno cambiati solo se ho accettazione mossa)
                appo = campiona.Get_mu();
                pippo = campiona.Get_sigma();

                //Propongo una nuova mossa
                campiona.Set_mu(appo + m_delta * (rand.Rannyu() - 0.5));
                campiona.Set_sigma(pippo + m_delta * (rand.Rannyu() - 0.5));

                //Calcolo l'integrale nella configurazione in cui mi trovo
                calcola.Integrale(campiona, 0);
                m_new = calcola.Get_integrale();

                p = exp(-m_beta * (m_new - m_old)); //Probabilità accettazione mossa
                if(rand.Rannyu() < p) { //Cambio effettivamente oppure no?
                    m_old = m_new;
                    fileout << m_old << "   " << calcola.Get_errore() << endl;
                    file_out << campiona.Get_mu() << "   " << campiona.Get_sigma() << endl;
                    acce++;
                }

                else{
                    //Riporto i parametri a quelli precedenti, se mossa non accetatta devo dimenticarmela
                    campiona.Set_mu(appo);
                    campiona.Set_sigma(pippo);
                }


            }


            cout << "Effettuato lavoro a T = " << T << endl;
            T = 0.9 * T;
        }

        fileout.close();
        file_out.close();

    }


    private:
    double T_in, T_fin, m_beta; //Data membri per temperatura
    double m_old, m_new;    //Data membri per valori integrali
    Random rand; double m_delta;   //Data membri per generazione delle mosse

};





#endif //__classi_h__