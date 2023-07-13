#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>

#include "TravelingSalesman.h"
#include "random.h"

#include "mpi.h"

using namespace std;

//Funzioni per decidere scambi
void Permuta(vector<int>& send, vector<int>& rec, Random& rnd);


int main(int argc, char* argv[]){

   //Variabili per parallelizzazione del calcolo
   int size, rank;
   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &size);
   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   MPI_Status stat[6];
   int itag[6] {0, 1, 2, 3, 4, 5};

   //Generatore di numeri casuali
   Random rnd;
   Init(rnd, rank);

   //Generatore numeri casuali condiviso
   Random rnd_cond;
   Init(rnd_cond, size);

   //Valori di input
   ifstream filein;
   int migr, nmigr, pop, ngen;

   filein.open("input.in");
   if(!filein) {
      cout << "Errore in apertura file di input." << endl;
      cout << "Esco!!" << endl;
      return 0;
   }
   filein >> migr >> nmigr >> pop >> ngen;
   filein.close();

   string nome = "AmericanCapitals.in";
   int n_city;

   TravelingSalesman test(rnd, nome, pop);
   n_city = test.Get_numc();

   if(migr == 0){
      test.GA(ngen, rnd, rank);
      test.StampaBest(rank);
   }

   else{

      //Vettore per decidere chi manda
      vector<int> send(size);
      vector<int> rec(size);
      vector<int> appo(n_city);
   
      for(int i=0; i<size; i++) { send[i] = i; }
      for(int i=0; i<size; i++) { rec[i] = i; }
      Permuta(send, rec, rnd_cond);

      int gen_m = (int)ngen/nmigr;

      //Evolvo per il numero di migrazioni che devo fare
      //Ogni volta faccio un numero gen_m di evoluzioni di GA
      for(int i=0; i<nmigr; i++) {
         
         //Effettuo un certo numero di step prima della prossima migrazione
         test.GA(gen_m, rnd, rank);
         //Salvo il vettore migliore in un file di appoggio
         appo = test.Get_primo();

         //Effettuo la migrazione
         MPI_Send(&appo[0], n_city, MPI_INTEGER, send[rank], itag[rank], MPI_COMM_WORLD);
         MPI_Recv(&appo[0], n_city, MPI_INTEGER, rec[rank], itag[rec[rank]], MPI_COMM_WORLD, &stat[rank]);

         //Setto i nuovi elementi delle varie popolazioni
         test.Set_primo(appo);
         //Cambio ordine di scambio per la prossima migrazione
         Permuta(send, rec, rnd_cond);
         //Riordino prima di eseguire nuovamente il GA
         test.Riordina();
         //Comunico
         //cout << "Eseguita migrazione numero: " << i + 1 << "  Nodo: " << rank << endl;

      }
      test.StampaBest(rank);
      
   }
   

   MPI_Finalize();
   rnd.SaveSeed();

return 0;
}


void Permuta(vector<int>& send, vector<int>& rec, Random& rnd) {
   int appo = 0;
   int ind1, ind2;

   while(appo == 0) {

      //Faccio gli scambi necessari   
      for(int i=0; i<(int)send.size(); i++) {
         ind1 = floor(rnd.Rannyu(0, send.size()));
         ind2 = floor(rnd.Rannyu(0, send.size()));

         appo = send[ind1];
         send[ind1] = send[ind2];
         send[ind2] = appo;

      }

      //Controllo su come ho generato i vettori(non voglio avere che un nodo manda a se stesso)
      for(int i=0; i<(int)send.size(); i++){
         appo *= (send[i] - i);
      }

   }

   //Inizializzo i vettori di receive
   for(int i=0; i<(int)send.size(); i++) {
      rec[send[i]] = i; 
   }
   
}
