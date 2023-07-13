/************************************************
*               Filippo Negrini                 *
*              Matricola: 965085                *
************************************************/

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

#include "random.h"

using namespace std;

//Funzione per inizializzazione di elemento di tipo Random
void init(Random& rnd);

class RandomWalk {

    public:
    //Costruttore di default
    RandomWalk() { m_a = 1., m_x = 0; m_y = 0; m_z = 0; m_appo = 0; init(m_rnd); }
    //Costruttore con input
    RandomWalk(double a, double x, double y, double z) { 
        m_a = a; m_x = x; m_y = y; m_z = z; m_appo = 0; init(m_rnd); 
    }
    //Distruttore
    ~RandomWalk() {;}

    //Metodi Set per membri privati
    void SetX(double x) { m_x = x; }
    void SetY(double y) { m_y = y; }
    void SetZ(double z) { m_z = z; }

    //Metodi Get per membri privati
    double GetX() { return m_x; }
    double GetY() { return m_y; }
    double GetZ() { return m_z; }

    //Metodo per salvare seme
    void SaveSeed() { m_rnd.SaveSeed(); }

    //Metodo per il calcolo della distanza2 dall'origine
    double Dist2() {return pow(m_x, 2) + pow(m_y, 2) + pow(m_z, 2); }

    //Metodo per step discreto
    void StepDiscr();
    //Metodo per step continuo
    void StepCont();

    private:
    double m_a; //Dimensione dello step
    double m_x, m_y, m_z; //Coordinate dell'estremo del random walk
    double m_appo; //Variabile d'appoggio
    Random m_rnd; //Classe per metodi random

};

//Funzione per decidere quale tipo di step (discreto oppure continuo) fare
void Step(int i, RandomWalk& cammino);

//Grafico RandomWalk step discreti
void GraficoDiscreto(RandomWalk& cammino, int l_RWalk);
//Grafico RandomWalk continuo
void GraficoContinuo(RandomWalk& cammino, int l_RWalk);