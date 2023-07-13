#ifndef __TravelingSalesman_h__
#define __TravelingSalesman_h__

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#include "random.h"

using namespace std;

//Funzione inizializzazione metodo random
void Init(Random& rnd, int rank);





/****************************************************
*        CLASSE MAPPA - PER DISPOSIZIONE CITTA'     *
****************************************************/
class Mappa {
    public:
    //Costruttore di default
    Mappa() {;}
    //Costruttore con input
    Mappa(string filename) {
        double appo {};
        
        ifstream filein;
        filein.open(filename);
        //Riempio vettore delle coordinate
        while(!filein.eof()) {
            filein >> appo;
            m_coord.push_back(appo);
        }
        m_ncity = m_coord.size()/2;

        //Una volta generati i punti scelgo di calcolare le distanze fra le varie città (in modo tale da non farlo un tot di volte)
        //Ottengo una matrice di dimensione m_ncity * m_ncity, mi basta calcolare metà degli elementi perchè matrice simmetrica
        m_costo.resize(m_ncity, vector<double>(m_ncity)); //Fornisco size contenitore, se no core dumped
        for(int i=0; i<m_ncity; i++) {

            for(int j=i; j<m_ncity; j++) {
                m_costo[i][j] = sqrt(pow(m_coord[2*i] - m_coord[2*j], 2) + pow(m_coord[2*i + 1] - m_coord[2*j + 1], 2));
                m_costo[j][i] = m_costo[i][j]; 
            }
        }

    }
    //Distruttore
    ~Mappa() {;}



    //Metodi Get
    int Get_num() { return m_ncity; }
    vector<double> Get_coord() { return m_coord; }
    double Get_coord(int i) { return m_coord[i]; }
    double Get_costo(int i, int j) { return m_costo[i][j]; }


    //Metodo per stampare a file coordinate
    void Stampa_coord(string nome) {
        ofstream fileout;
        fileout.open(nome);

        //Controllo su apertura file
        if(!fileout) {   
            cerr << "Problema in apertura file: " + nome << endl; 
            cerr << "Non stampo a file le coordinate!!" << endl;
            }

        else{
            for(int i=0; i<m_ncity; i++) {
                fileout << m_coord[2*i] << "   " << m_coord[2*i + 1] << endl;
            }
            fileout.close();

            //Commento a terminale
            cout << "Coordinate stampate sul file: "  + nome << endl;
        }

    }

    void Stampa_costo(string nome) {
        ofstream fileout;
        fileout.open(nome);

        //Controllo su apertura file
        if(!fileout) {   
            cerr << "Problema in apertura file: " + nome << endl; 
            cerr << "Non stampo a file costo!" << endl;
        }

        else{

            for(int i=0; i<m_ncity; i++) {
                for(int j=0; j<m_ncity; j++) {
                    fileout << m_costo[i][j] << "   ";
                }
                fileout << endl;
            }
            fileout.close();

            //Commento a terminale
            cout << "Coordinate stampate sul file: "  + nome << endl;
        }

    }

    protected:
    int m_ncity;    //Numero di città
    vector<double>  m_coord; //Coordinate dei punti
    vector<vector<double>> m_costo;    //Distanze fra punti (squared)

};








/****************************************************
*      CLASSE CAMMINO - PER DISPOSIZIONE CITTA'     *
****************************************************/
class Cammino {
    public:
    //Costruttore di default
    Cammino() {;}
    //Costruttore con input
    Cammino(Random& rnd, Mappa& mappa) {
        
        int ind_1, ind_2;
        m_lengh = mappa.Get_num();
        //Riempio il vettore con sequenza ordinata da 0 a 49
        for(int i=0; i<m_lengh; i++) {  m_seq.push_back(i);}

        //Effettuo swap per generare una sequenza "casuale" - ne effettuo len senza andare a coinvolgere il primo valore della seq
        for(int i=0; i<m_lengh; i++) {
            ind_1 = floor(rnd.Rannyu(1, m_lengh));
            do{ ind_2 = floor(rnd.Rannyu(1, m_lengh)); }while(ind_1 == ind_2);

            //Effettuo scambio
            Scambia(ind_1, ind_2);
        }

        //Controllo di non aver sgravato
        Controllo();
        Costo_V(mappa);

    }

    //Metodi Get
    double Get_costo() { return m_costo; }
    int Get_len() { return m_lengh; }
    vector<int> Get_seq() { return m_seq; }
    int Get_Val(int i) { return m_seq[i]; }

    //Metodi set
    void Set_Val(int ind, int val) { 
        if(ind >= (int)m_seq.size()) {
            cerr << "Indice selezionato troppo grande" << endl;
            cerr << "Non eseguo modifica del contenuto" << endl;
        }

        else {
            m_seq[ind] = val;
        }
    } 
    
    void Set_seq(vector<int>& cand) {
        if((int)cand.size() != m_lengh) {
            cerr << "Dimensioni differenti: non effettuo lo scambio" << endl;
        }

        else{
            for(int i=0; i<m_lengh; i++) {
                m_seq[i] = cand[i];
            }
        }
    }

    //Sequenza di controllo
    void Controllo() {
        /*Controllo due cose:
            1 -> città di partenza è la numero 1, voglio evitare eccessiva ridondanza
            2 -> ho passaggio per la stessa città
        */

        //Da dove parto?
        if(m_seq[0] != 0) {
            cerr << "La sequenza non inizia dalla prima città!!" << endl;
            cerr << "Errore in codice" << endl;
        }

        //Ho ripetizioni nella sequenza
        for(int i=0; i<m_lengh-1; i++) {
            for(int j=i+1; j<m_lengh; j++) {
                if(m_seq[i] == m_seq[j]) {
                    cerr << "Errore: due città uguali fra loro!!" << endl;
                    cerr << "Esco" << endl;
                    break;
                }
            }
        }
    }

    
    //Funzione costo (void)
    void Costo_V(Mappa& disposizione) {
        m_costo = 0;
        for(int i=0; i<m_lengh-1; i++) {
            m_costo += disposizione.Get_costo(m_seq[i], m_seq[i+1]);    //Se mi fermo qui non torno a casa
        }
        m_costo += disposizione.Get_costo(m_seq[m_lengh-1], 0); //Torno a casa
    }


    //Funzione costo (restituisce somma distanze quadre fra città)
    double Costo_R(Mappa& disposizione) {
        m_costo = 0;
        for(int i=0; i<m_lengh-1; i++) {
            m_costo += disposizione.Get_costo(m_seq[i], m_seq[i+1]);    //Se mi fermo qui non torno a casa
        }
        m_costo += disposizione.Get_costo(m_seq[m_lengh-1], m_seq[0]); //Torno a casa

        return m_costo;
    }

    //Metodo per effettuare swap
    void Scambia(int i, int j) {
        
        int appo;
        //Controllo se i diverso da j
        if(i == j) { 
            cerr << "Stesso indice per lo swap: errore" << endl; 
            cerr << "Non effettuo lo scambio!" << endl;
        }

        else{
            //Effettuo lo scambio
            appo = m_seq[i];
            m_seq[i] = m_seq[j];
            m_seq[j] = appo;
        }

        Controllo();
    }

    //Metodo per scambiare sequenza lunga len con adiacente lunga len
    void Scambia_seq(int iniz, int len) {
        //La lunghezza della sequenza deve essere minore di N/2
        if(len >= m_lengh/2) {
            cerr << "Sequenza selezionata è troppo lunga!!" << endl;
            cerr << "Non effettuo scambio sequenze" << endl;
        }

        //Se passo il primo test provo a effettuare lo spostamento
        else {
            int appo {}; //Variabile d'appoggio
            int appo1 {}; //Variabile d'appoggio

            for(int i = iniz; i<iniz + len; i++) {
                appo = i; appo1 = i + len;

                //Controllo se devo tornare indietro
                if(appo > (m_lengh - 1)) { appo = i%(m_lengh-1); }
                if(appo1 > (m_lengh - 1)) { appo1 = (i+len)%(m_lengh-1); }

                //Effettuo lo scambio
                Scambia(appo, appo1);
            }

            Controllo();
        }
    }

    //Metodo per scambiare una sequenza lunga len con una non adiacente
    void Scambia_seq(int iniz, int len, int dist) {

        //La lunghezza della sequenza deve essere minore di (len - dist)/2
        if(len >= (m_lengh - dist)/2) {
            cerr << "Sequenza selezionata è troppo lunga!!" << endl;
        }

        else{
            int appo {}; //Variabile d'appoggio
            int appo1 {}; //Variabile d'appoggio

            for(int i = iniz; i<iniz + len; i++) {
                appo = i; appo1 = i + len + dist;

                //Controllo se devo tornare indietro
                if(appo > (m_lengh - 1)) { appo = i%(m_lengh-1); }
                if(appo1 > (m_lengh - 1)) { appo1 = (i+len+dist)%(m_lengh-1); }

                //Effettuo lo scambio
                Scambia(appo, appo1);
            }

            Controllo();
        }
            
    }

    //Metodo per spostare una sequenza
    void Sposta_seq(int iniz, int len, int dist) {

        if(len >= (m_lengh - 1)) {
            cerr << "Sequenza selezionata è troppo lunga!!" << endl;
        }

        else{
            int ind1, ind2; //Per possibili problemi di sequenza lunga
            vector<double> appo;    //Vettore contenitore

            //Mi salvo parte che voglio spostare in avanti
            for(int i=iniz; i< iniz+len; i++) { 
                ind1 = i;
                if(ind1 > (m_lengh - 1)) {ind1 = i%(m_lengh-1);}

                appo.push_back(m_seq[ind1]); 
            }
            
            //Porto indietro la parte che shifta
            for(int i=iniz+len; i<iniz+len+dist; i++) { 
                ind1 = i; ind2 = i-len;
                if(ind1 > (m_lengh - 1)) {ind1 = i%(m_lengh-1);}
                if(ind2 > (m_lengh - 1)) {ind2 = (i-len)%(m_lengh-1);}

                m_seq[ind2] = m_seq[ind1]; 
            }

            //Posiziono la parte salvata in appo;
            for(int i=iniz+dist; i<iniz+dist+len; i++) { 
                ind1 = i;
                if(ind1 > (m_lengh - 1)) {ind1 = i%(m_lengh-1);}

                m_seq[ind1] = appo[i-iniz-dist]; 
            }

            Controllo();
        }

    }

    //Metodo per invertire senquenza città
    void Inverti(int iniz, int len) {

        if(len > (m_lengh - 1)) {
            cerr << "Sequenza selezionata è troppo lunga!!" << endl;
        }

        else{
            int conta = 0;  //Contatore
            int ind1, ind2; //Per possibili problemi di sequenza lunga

            ind1 = iniz; ind2 = iniz + len - 1;
            while(ind1 < ind2) {
                if(ind1 > (m_lengh - 1)) {ind1 = ind1%(m_lengh-1);}
                if(ind2 > (m_lengh - 1)) {ind2 = ind2%(m_lengh-1);}

                //Effettuo lo scambio
                Scambia(ind1, ind2);

                conta++; //Faccio update contatore
                ind1 = iniz + conta; ind2 = iniz + len - 1 - conta; //Faccio update indici
            }
            
            Controllo();
        }

    }

    //Metodo per stampare la sequenza
    void Stampa() {
        for(int i=0; i<m_lengh; i++) { cout << m_seq[i] << "    "; }
        cout << endl;
    }
    
    //Metodo per stampare la sequenza (a file)
    void Stampa(string filename) {
        ofstream fileout;
        fileout.open(filename);

        if(!fileout) {
            cerr << "Problemi in apertura file di output" << endl;
            cerr << "Non stampo ordine delle città cammino" << endl;
        }

        else{
            for(int i=0; i<m_lengh; i++) { fileout << m_seq[i] << "    "; }
            cout << "Ordine delle città stampato su: " << filename << endl;
        }
        fileout << endl;
    }

    void Stampa_coord(string nome, Mappa& pos) {
        ofstream fileout;
        fileout.open(nome);
        if(!fileout) { 
            cout << "Errore in apertura file" << endl;
            cout << "Non stampo coordinate del cammino" << endl;
        }

        else{
            for(int i=0; i<m_lengh; i++) {
                fileout << pos.Get_coord(2 * m_seq[i]) << "     " << pos.Get_coord(2*m_seq[i]+1) << endl;
            }
            fileout << pos.Get_coord(2*m_seq[0]) << "     " << pos.Get_coord(2*m_seq[0]+1) << endl;
        }
        fileout.close();
    }

    protected:
    int m_lengh;
    double m_costo;
    vector<int> m_seq;

};









/************************************************************
*        CLASSE TRAVELING SALESMAN - ALGORITMO GENETICO     *
************************************************************/
class TravelingSalesman{
    public:
    //Costruttore di default
    TravelingSalesman() {;}
    //Costruttore con input
    TravelingSalesman(Random& rnd, string nome, int ncamm) {
        m_ncamm = ncamm; //Numero di cammini

        //Lavoro con la mappa
        Mappa disp(nome);
        m_disp = disp;
        m_ncity = disp.Get_num();

        popo.resize(m_ncamm);
        for(int i=0; i<m_ncamm; i++) { //Inizializzo la popolazione
            Cammino test(rnd, m_disp);
            popo[i] = test;
        }
        

        Riordina(); //Riordino che così parto già da ordinati
    }
    //Distruttore
    ~TravelingSalesman() {;}

    //Effettuo le mutazioni (la prima avviene con probabilità 10%, una seconda con probabilità 5%, terza con 3% e quarta con 1%)
    //Rarissimamente il mio cammino avrà più di due mutazioni
    void Mutazione(int i, Random& rnd) {
        double appo;
        int conta = 0;
        int ind1, ind2, mut;
        
        //Ogni volta provo a effettuare una mutazione estraendo in modo casuale un numero fra 0 e 3
        //Potenzialmente posso effettuare la stessa mutazione più volte durante la stessa evoluzione
        //Lunghezza mutazione scelta a caso fra 5 e 10
        //Posizione iniziale scelta a caso
        for(int j=0; j<4; j++) {
            //Scelgo mutazione da fare
            mut = floor(rnd.Rannyu(0, 4));

            //Scambio
            if(mut == 0) {
                appo = rnd.Rannyu();
                if(appo < m_prob[conta]) { 
                    ind1 = floor(rnd.Rannyu(1, m_ncity));
                    do{ ind2 = floor(rnd.Rannyu(1, m_ncity)); }while(ind1 == ind2);
                    popo[i].Scambia(ind1, ind2); 
                    conta++; 
                }
            }

            //Scambio sequenze adiacenti
            else if(mut == 1) {
                appo = rnd.Rannyu();
                if(appo < m_prob[conta]) {   
                    popo[i].Scambia_seq(floor(rnd.Rannyu(1, m_ncity)), floor(rnd.Rannyu(2, m_ncity/2)));
                    conta++;
                }
                
            }

            //Sposto di una certa distanza sequenza
            else if(mut == 2) {
                appo = rnd.Rannyu();
                if(appo < m_prob[conta]) {   
                    ind1 = floor(rnd.Rannyu(2, m_ncity-1));
                    ind2 = floor(rnd.Rannyu(1, m_ncity-ind1));
                    popo[i].Sposta_seq(floor(rnd.Rannyu(1, m_ncity)), ind1, ind2); 
                    conta++;
                }
                
            }

            //Inverto sequenza
            else if(mut == 3) {   
                appo = rnd.Rannyu();
                if(appo < m_prob[conta]) { 
                    popo[i].Inverti(floor(rnd.Rannyu(1, m_ncity)), floor(rnd.Rannyu(2, m_ncity))); 
                    conta++;
                }
            }
        }

        //Effettuo una nuova stima del costo del percorso (solo se ho effettuato dei cambi)
        if(conta != 0) { popo[i].Costo_V(m_disp); }
            

    }

    //Metodi Get
    int Get_numc() {return m_ncity; }
    vector<int> Get_primo(){ return popo[0].Get_seq(); }
    double Get_costo(int i) {   return popo[i].Get_costo(); }   //Ritorna costo di un elemento a scelta della popolazione
    double Get_best() { return m_best;  }   //Ritorna il costo del migliore
    double Get_Av() {   return m_av;    }  //Metodo che ritorna la media della first half della popolazione
    
    //Metodi Set
    void Set_primo(vector<int>& cand) { popo[0].Set_seq(cand); }

    //Metodo per ordinare il vettore di popolazioni
    void Riordina() {
        int ind;
        Cammino appo;
        
        for(int i=0; i<(int)popo.size()-1; i++) {
            ind = i; //Indice di partenza dell'algoritmo

            for(int j=i+1; j<(int)popo.size(); j++) {
                //Vedo se percorso è più corto di quello selezionato
                if(popo[j].Get_costo() < popo[ind].Get_costo()) { ind = j; }
            }

            //Effettuo lo scambio
            appo = popo[i];
            popo[i] = popo[ind];
            popo[ind] = appo;

        }

        m_best = popo[0].Get_costo();   //Percorso migliore

        m_av = 0;
        for(int i=0; i<m_ncamm/2; i++) { m_av = m_av * i/(i+1) + Get_costo(i)/(i+1); }  //Media sulla prima metà dei percorsi


    }

    //Metodo per selezione genetica
    void GA(int n_gen, Random& rnd, int rank) {
        int appo = 0;
        int pippo = 0;      
        ofstream fileout, file_out;

        fileout.open("best" + to_string(rank) + ".out.dat", ios::app);
        file_out.open("mean" + to_string(rank) + ".out.dat", ios::app);

        //Controllo su apertura file
        if(!file_out || !fileout) {
            cerr << "Errore in apertura file output" << endl;
            cerr << "Non eseguo codice genetico" << endl;
        }

        else {
            //Ciclo per il numero di generazioni richiesto
            for(int i=0; i<n_gen; i++) {

                //Effettuo un numero di Mating pari a N/2 (non può riprodursi con se stesso)
                //Nel mondo dei cammini possono esserci tradimenti, ma non riproduzione asessuata
                for(int i=0; i<m_ncamm/2; i++) {
                    appo = floor(m_ncamm * pow(rnd.Rannyu(), 4)); //Indice per primo genitore
                    do{ pippo = floor(m_ncamm * pow(rnd.Rannyu(), 4)); }while(appo == pippo);//Indice per secondo genitore

                    Cammino figl1 = popo[appo];  //Primo candidato figlio
                    Cammino figl2 = popo[pippo]; //Secondo candidato figlio

                    //Scelgo se fare il crossover
                    if(rnd.Rannyu() < 0.7) {   CrossOver(figl1, figl2, rnd);   }

                    //Appendo a popo i figli (alla fine avrò allora un contenitore lungo 2 * m_ncamm)
                    popo.push_back(figl1); popo.push_back(figl2);   //Aggiungo in contenitore
                    Mutazione(m_ncamm + 2*i, rnd); Mutazione(m_ncamm + 2*i + 1, rnd); //Provo a farli mutare

                }

                //Riordino tutto in merito alla lunghezza
                Riordina();

                //Tengo solo le prime m_ncamm componenti di popo
                popo.resize(m_ncamm);

                fileout << i+1 << "   " << m_best << endl;
                file_out << i+1 << "   " << m_av << endl;
            }
        }

        fileout.close(); file_out.close();

    }

    //Metodo per cross-over
    void CrossOver(Cammino& figl1, Cammino& figl2, Random& rnd) {
        int ind, appo;

        //Contenitori per sequenze
        vector<double> appo1;
        vector<double> appo2;
        
        //Seleziono casualmente un indice
        //Indice selezionato è fra 1 e figl*.size() - 2
        //Tronco dopo la casella che persenta l'indice selezionato
        //Se troncassi a penultima o alla prima otterrei gli stessi figli di partenza - non la voglio come cosa
        ind = floor(rnd.Rannyu(1, (int)figl1.Get_len() - 2));

        //Ordine nuovo degli elementi
        for(int j=1; j<figl1.Get_len(); j++) {
            appo = figl2.Get_Val(j);
            for(int i=ind + 1; i<figl1.Get_len(); i++) {
                if(appo == figl1.Get_Val(i)) { appo1.push_back(appo); }
            }
        }

        //Ordine nuovo degli elementi
        for(int j=1; j<figl1.Get_len(); j++) {
            appo = figl1.Get_Val(j);
            for(int i=ind + 1; i<figl1.Get_len(); i++) {
                if(appo == figl2.Get_Val(i)) { appo2.push_back(appo); }
            }
        }

        //Scambio gli elementi
        for(int i=ind+1; i<figl1.Get_len(); i++) {
            figl1.Set_Val(i, appo1[i - ind - 1]);
            figl2.Set_Val(i, appo2[i - ind - 1]);
        }

        figl1.Controllo();
        figl2.Controllo();

    }

    //Metodo che stampa le coordinate del miglior percorso
    void StampaBest(int rank) {
        popo[0].Stampa_coord("best_config" + to_string(rank) + ".out.dat", m_disp);
    }




    protected:
    int m_ncity; //Numero di città
    int m_ncamm; //Numero di abitanti popolazione
    Mappa m_disp; //Mappa delle città
    double m_best, m_av;
    vector<Cammino> popo; //Vettore che contiene popolazione
    vector<double> m_prob {0.1, 0.05, 0.03, 0.01}; //Vettore con probabilità mutazioni successive
};

#endif //__TravelingSalesman_h__