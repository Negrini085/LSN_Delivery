/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#include "funzioni.h"


//Funzione per estrarre una x distribuita come: p(x) = (6/5)*[1-(x^2)/2] (con AR)
double Distr1_AR(Random& rnd) {
   double appo = 0;
   double pippo = 0;

   do{
      //Determino i due valori random che sono necessari per effettuare il test accept-reject
      appo = rnd.Rannyu();
      pippo = rnd.Rannyu(0, (double)6/5);

   }while(pippo > 6 * (1 - pow(appo,2)/2)/5.);

   return appo;
}



//Funzione per estrarre una x distribuita come: p(x) = (6/5)*[1-(x^2)/2] (con metodo ibrido)
double Distr1_Hyb(Random& rnd) {

    double appo = 0;
    double pippo = 0;

    do{
        //Determino valori necessari per metodo accept - reject
        appo = 2. - sqrt(4 - 3 * rnd.Rannyu());
        pippo = rnd.Rannyu();
    }while(pippo > 3 * (2 - pow(appo, 2)) / (4 * (2 - appo)));

    return appo;
}



//Funzione per estrarre una x distribuita come: p(x) = 2 * (1 - x) (con metodo cumulativa)
double Distr2_Cum(Random& rnd) {
    return 1 - sqrt(1 - rnd.Rannyu());
}



//Funzione per test delle distribuzioni di probabilità
void test(Random& rnd) {

    //Prima riga conterrà test prima distribuzione, seconda riga test seconda distr. e così via
    ofstream fileout;
    fileout.open("Test.dat");

    if(!fileout) {
        cerr << "Problemi in apertura file output per test distribuzioni!" << endl;
    }

    for(int i=0; i<3; i++) {
    
        //Faccio test delle varie probabilità
        if(i == 0) { for(int j=0; j<100000; j++) { fileout << Distr1_AR(rnd) << "   "; } }

        else if(i == 1) { for(int j=0; j<100000; j++) { fileout << Distr1_Hyb(rnd) << "    "; }}

        else if(i == 2) { for(int j=0; j<100000; j++) { fileout << Distr2_Cum(rnd) << "    "; }}

        fileout << endl;
    }

    fileout.close();
}



//Funzione per la singola stima dell'integrale (distribuzione uniforme)
double stima_unif(int n_throws, Random& rnd) {

    double appo = 0; //Variabile d'appoggio
    
    //Ciclo for per calcolare lo stimatore dell'integrale
    for(int i=0; i<n_throws; i++) {
        appo = appo * i/(i+1) + M_PI * cos(M_PI * rnd.Rannyu()/2)/(2 * (i+1));
    }

    return appo;
}



//Funzione per la singola stima dell'integrale (distribuzione p(x) = (6/5)*[1-(x^2)/2])
double stima_distr1AR(int n_throws, Random& rnd) {

    double appo = 0; //Variabile d'appoggio
    double pippo = 0; //Variabile d'appoggio

    //Ciclo for per calcolare lo stimatore dell'integrale
    for(int i=0; i<n_throws; i++) {
        pippo = Distr1_AR(rnd);
        appo = appo * i/(i+1) + M_PI * cos(M_PI * pippo/2)/(12 * (i+1) * (1 - pow(pippo, 2)/2.)/5);
    }

    return appo;
}



//Funzione per la singola stima dell'integrale (distribuzione p(x) = (6/5)*[1-(x^2)/2] con metodo ibrido)
double stima_distr1Hyb(int n_throws, Random& rnd) {
    
    double appo = 0; //Variabile d'appoggio
    double pippo = 0; //Variabile d'appoggio

    //Ciclo for per calcolare lo stimatore dell'integrale
    for(int i=0; i<n_throws; i++) {
        pippo = Distr1_Hyb(rnd);
        appo = appo * i/(i+1) + M_PI * cos(M_PI * pippo/2)/(12 * (i+1) * (1 - pow(pippo, 2)/2.)/5);
    }

    return appo;
}



//Funzione per la singola stima dell'integrale (distribuzione p(x) = 2 * (1 - x) cumulativa)
double stima_distr2Cum(int n_throws, Random& rnd) {
    
    double appo = 0; //Variabile d'appoggio
    double pippo = 0; //Variabile d'appoggio
    
    //Ciclo for per calcolare lo stimatore dell'integrale
    for(int i=0; i<n_throws; i++) {
        pippo = Distr2_Cum(rnd); //Valore x_i estratt0
        appo = appo * i/(i+1) + M_PI * cos(M_PI * (pippo)/2)/(4 * (i+1) * (1 - pippo));
    }

    return appo;
}



//Funzione per stima integrale
double stima(int n_throws, Random& rnd, int& i) {

    //Caso utilizzo distribuzione uniforme
    if(i == 0) { return stima_unif(n_throws, rnd); }

    else if(i == 1) { return stima_distr1AR(n_throws, rnd); }

    else if(i == 2) { return stima_distr1Hyb(n_throws, rnd); }

    else if(i == 3) { return stima_distr2Cum(n_throws, rnd); }

    return 1;
}

