#include "funzioni.h"


//Funzione per calcolare la lunghezza di un vettore uscente da (0,0)
double lunghezza(double a, double b) {
   return sqrt(pow(a, 2) + pow(b,2));
}

//Funzione che ritorna le coordinate dei due estremi dell'ago
vector<double> ago_random(Random& rnd, double dist_righe, double lungh_ago) {
   
   vector<double> coordinate; //Vettore per coordinate y estremi
   double appo, pippo, lungh;

   //Genero coordinata y primo estremo
   coordinate.push_back(rnd.Rannyu(0, dist_righe));


   //Determino l'orientamento spaziale dell'ago
   //Per fare questo in primo luogo trovo random le coordinate di un vettore unitario (scarto se oltre la circonferenza)
   do{
      
      appo = rnd.Rannyu(-1,1); //Coordinata x
      pippo = rnd.Rannyu(-1,1); //Coordinata y

      lungh = lunghezza(appo, pippo); //Lunghezza vettore sparato in quadrato [-1,1] x [-1,1]
   }while(lungh > 1);

   //Aggiorno valore della coordinata y
   pippo /= lungh;

   //Valuto ora correttamente quanto valga la y del secondo estremo
   coordinate.push_back(coordinate[0] + lungh_ago * pippo);

   return coordinate;
}



//Funzione che ritorna la singola stima di pi greco
double stima_pi(Random& rnd, double dist_righe, double lungh_ago, double n_throws) {

   int n_hit = 0; //Contenitore per il numero di hit
   vector<double> coordinate; //Vector per contenere coordinate y estremi ago lanciato

   for(int i=0; i<n_throws; i++) {
      coordinate = ago_random(rnd, dist_righe, lungh_ago);
      
      //Controllo se ho hit oppure no
      if(coordinate[1] <= 0 || coordinate[1] >= dist_righe) { n_hit++;}

   }

   return 2 * lungh_ago * n_throws / (n_hit * dist_righe);
}


//Funzione per la stima hit/miss di pi_greco
double pi_HitMiss(Random& rnd, int n_throws) {

   int n_hit = 0; //Contenitore per il numero di hit
   vector<double> coordinate {0, 0}; //Vector per contenere coordinate punto

   for(int i=0; i<n_throws; i++) {

      //Tiro random le coordinate di un punto appartenente alla regione di piano [-1, +1] x [-1, +1]
      coordinate[0] = rnd.Rannyu(-1, 1);
      coordinate[1] = rnd.Rannyu(-1, 1);

      //Testo condizione per la hit
      if(lunghezza(coordinate[0], coordinate[1]) <= 1) { n_hit++; }

   }

   return 4 * (double)n_hit/n_throws;
}