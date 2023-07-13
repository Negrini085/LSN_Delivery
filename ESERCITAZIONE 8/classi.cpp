#include "classi.h"



//Funzione per inizializzazione di elemento di tipo Random
void init(Random& rnd) {
    
    int seed[4];
    int p1, p2;
    ifstream Primes("../RANDOM_GEN/Primes");
    if (Primes.is_open()){
        Primes >> p1 >> p2 ;
    } else cerr << "PROBLEM: Unable to open Primes" << endl;
    Primes.close();

    ifstream input("../RANDOM_GEN/seed.in");
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



//Funzione per calcolo del modulo quadro
double mod2(double x, double sigma, double mu) {
    double appo = 0;

    //Valore della funzione d'onda nel punto x considerato
    appo = exp(-pow(x-mu, 2)/(2 * pow(sigma, 2))) + exp(-pow(x+mu, 2)/(2 * pow(sigma, 2)));

    //Restituisco il valore del quadrato della funzione d'onda
    return appo*appo;
}



//Integranda
double integranda(double x, double sigma, double mu) {

    double appo = 0;

    //Effettuo il calcolo
    appo = (pow(sigma, 2) - pow(mu, 2) - pow(x, 2) + 2*x*mu*tanh(x*mu/pow(sigma, 2)))/(2*pow(sigma, 4)) + pow(x, 4) - 2.5 * pow(x, 2);

    return appo;
}