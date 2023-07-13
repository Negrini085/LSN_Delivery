/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/

#include <iostream>
#include <fstream>
#include <ostream>
#include <cmath>
#include <iomanip>

#include "Monte_Carlo_ISING_1D.h"

using namespace std;

int main()
{ 
  Input(); //Inizialization

  for(int i=0; i<3000; i++) { Move(metro); }

  for(int iblk=1; iblk <= nblk; ++iblk) //Simulation
  {
    Reset(iblk);   //Reset block averages
    for(int istep=1; istep <= nstep; ++istep)
    {
      Move(metro);
      Measure();
      Accumulate(); //Update block averages
    }
    Averages(iblk);   //Print results for current block
  }
  ConfFinal(); //Write final configuration

  return 0;
}


void Input(void)
{
  ifstream ReadInput;

  cout << "Classic 1D Ising model             " << endl;
  cout << "Monte Carlo simulation             " << endl << endl;
  cout << "Nearest neighbour interaction      " << endl << endl;
  cout << "Boltzmann weight exp(- beta * H ), beta = 1/T " << endl << endl;
  cout << "The program uses k_B=1 and mu_B=1 units " << endl;

//Read seed for random numbers
   int p1, p2;
   ifstream Primes("../RANDOM_GEN/Primes");
  if (Primes.is_open()){
    Primes >> p1 >> p2 ;
  } else cerr << "PROBLEM: Unable to open Primes" << endl;
  Primes.close();

   ifstream input("../RANDOM_GEN/seed.in");
   input >> seed[0] >> seed[1] >> seed[2] >> seed[3];
   rnd.SetRandom(seed,p1,p2);
   input.close();
  
//Read input informations
  ReadInput.open("input.dat");

  ReadInput >> temp;
  bet_a = 1.0/temp;
  cout << "Temperature = " << temp << endl;

  ReadInput >> nspin;
  cout << "Number of spins = " << nspin << endl;

  ReadInput >> J;
  cout << "Exchange interaction = " << J << endl;

  ReadInput >> h;
  cout << "External field = " << h << endl << endl;
    
  ReadInput >> metro; // if=1 Metropolis else Gibbs

  ReadInput >> nblk;

  ReadInput >> nstep;

  if(metro==1) cout << "The program perform Metropolis moves" << endl;
  else cout << "The program perform Gibbs moves" << endl;

  cout << "Number of blocks = " << nblk << endl;
  cout << "Number of steps in one block = " << nstep << endl << endl;
  ReadInput.close();


//Prepare arrays for measurements
  iu = 0; //Energy
  ic = 1; //Heat capacity
  im = 2; //Magnetization
  ix = 3; //Magnetic susceptibility
 
  n_props = 4; //Number of observables

//initial configuration
  for (int i=0; i<nspin; ++i)
  {
    //s[i] = 1;
    if(rnd.Rannyu() >= 0.5) s[i] = 1;
    else s[i] = -1;
  }

//Stampo la configurazione iniziale a file
  ofstream fileout;
  fileout.open("config_in.dat", ios::app);
  for(int i=0; i<nspin; i++) { fileout << s[i] << "  "; }
  fileout << endl;
  fileout.close();
  
//Evaluate energy etc. of the initial configuration
  Measure();

  //Print initial values for measured properties
  cout << "Initial energy per particle      = " << walker[iu]/(double)nspin << endl;
  cout << "Initial magnetization            = " << walker[im] << endl << endl;

  cout << "----------------------------" << endl << endl;

}


void Move(int metro)
{
  int o;
  double p, energy_old, energy_new, sm;
  double energy_up, energy_down, delta_en;

  for(int i=0; i<nspin; i++) {

      //Select randomly a particle (for C++ syntax, 0 <= o <= nspin-1)
    o = (int)(rnd.Rannyu()*nspin);

    //Metodo Metropolis
    if(metro==1) { 

      energy_old = walker[iu]; //Energia vecchia configurazione

      sm = s[o] * -1; //Spin flippato
      energy_up = Boltzmann(s[o], o);
      energy_down = Boltzmann(sm, o);
      energy_new = energy_old - energy_up + energy_down; //Energia configurazione con spin flippato

      //Valuto probabilita accettazione mossa
      p = min(1., exp(-bet_a * (energy_new - energy_old))); 

      //Accettazione
      if(rnd.Rannyu() < p) {
        s[o] = sm; //Aggiorno la configurazione
        accepted++; //Aumento numero di switch accettati
      }

    }
    else //Gibbs sampling
    {
      energy_up   = Boltzmann(1,o); //Valuto energia con spin up
      energy_down = Boltzmann(-1,o);  //Valuto energia con spin down

      p = 1/(1+exp(bet_a*(energy_up-energy_down))); //Probabilità di avere mossa

      if(rnd.Rannyu()<p){ s[o]=1; } //Caso up
      else{ s[o]=-1; }  //Caso down
        
      accepted++;
      
    }

    attempted++;

  }

}

double Boltzmann(int sm, int ip)
{
  double ene = -J * sm * ( s[Pbc(ip-1)] + s[Pbc(ip+1)] ) - h * sm;
  return ene;
}

void Measure()
{
  int bin;
  double u = 0.0; //Calcolo dell'energia
  double m = 0.0; //Calcolo della magnetizzazione

//Ciclo sugli spin
  for (int i=0; i<nspin; ++i)
  {
     u += -J * s[i] * s[Pbc(i+1)] - 0.5 * h * (s[i] + s[Pbc(i+1)]);
     m += s[i];
  }
  walker[iu] = u; //Aggiorno energia
  walker[ic] = u*u; //Immagazzino energia2
  walker[im] = m; //Aggiorno magnetizzazione
  walker[ix] = m*m; //Immagazzino magnetizzazione2
}


void Reset(int iblk) //Reset block averages
{
   
   if(iblk == 1)
   {
       for(int i=0; i<n_props; ++i)
       {
           glob_av[i] = 0;
           glob_av2[i] = 0;
       }
   }

   for(int i=0; i<n_props; ++i)
   {
     blk_av[i] = 0;
   }
   blk_norm = 0;
   attempted = 0;
   accepted = 0;
}


void Accumulate(void) //Update block averages
{

   for(int i=0; i<n_props; ++i)
   {
     blk_av[i] = blk_av[i] + walker[i];
   }
   blk_norm = blk_norm + 1.0;
}


void Averages(int iblk) //Print results for current block
{
    
   ofstream Ene, Heat, Mag, Chi;
   const int wd=12;
    
    cout << "Block number " << iblk << endl;
    cout << "Acceptance rate " << accepted/attempted << endl << endl;

    //File per comunicazione ad output    
    Ene.open("output.ene.0",ios::app);
    Heat.open("output.heat.0",ios::app);
    Mag.open("output.magn.0",ios::app);
    Chi.open("output.chi.0",ios::app);



    /**********************************
    *         Energia interna         *
    **********************************/
    stima_u = blk_av[iu]/blk_norm/(double)nspin;
    glob_av[iu]  += stima_u;
    glob_av2[iu] += stima_u*stima_u;
    err_u = Error(glob_av[iu],glob_av2[iu],iblk);



    /********************************
    *       Calore specifico        *
    ********************************/
    stima_c = pow(bet_a, 2) * (blk_av[ic]/blk_norm - pow(stima_u * (double)nspin, 2))/nspin;
    glob_av[ic] += stima_c;
    glob_av2[ic] += stima_c*stima_c;
    err_c = Error(glob_av[ic], glob_av2[ic], iblk);



    /********************************
    *        Magnetizzazione        *
    ********************************/
    stima_m = blk_av[im]/blk_norm/(double)nspin;
    glob_av[im] += stima_m;
    glob_av2[im] += stima_m*stima_m;
    err_m = Error(glob_av[im], glob_av2[im], iblk);



    /*******************************
    *   Suscettibilità magnetica   *
    *******************************/
    stima_x = bet_a * (blk_av[ix]/blk_norm/(double)nspin - pow(stima_m, 2));
    glob_av[ix] += stima_x;
    glob_av2[ix] += stima_x*stima_x;
    err_x = Error(glob_av[ix], glob_av2[ix], iblk);


    //Stampo a file
    Ene << " " << setw(wd) << iblk << " " << setw(wd) << stima_u << " " << setw(wd) << glob_av[iu]/(double)iblk << " " << setw(wd) << err_u << endl;
    Heat << " " << setw(wd) << iblk << " " << setw(wd) << stima_c << " " << setw(wd) << glob_av[ic]/(double)iblk << " " << setw(wd) << err_c << endl;
    Mag << " " << setw(wd) << iblk << " " << setw(wd) << stima_m << " " << setw(wd) << glob_av[im]/(double)iblk << " " << setw(wd) << err_m << endl;
    Chi << " " << setw(wd) << iblk << " " << setw(wd) << stima_x << " " << setw(wd) << glob_av[ix]/(double)iblk << " " << setw(wd) << err_x << endl;


    Ene.close();
    Heat.close();
    Mag.close();
    Chi.close();

    cout << "----------------------------" << endl << endl;
}


void ConfFinal(void)
{
  ofstream WriteConf;

  cout << "Print final configuration to file config.final " << endl << endl;
  WriteConf.open("config.final");
  for (int i=0; i<nspin; ++i)
  {
    WriteConf << s[i] << endl;
  }
  WriteConf.close();

  rnd.SaveSeed();
}

int Pbc(int i)  //Algorithm for periodic boundary conditions
{
    if(i >= nspin) i = i - nspin;
    else if(i < 0) i = i + nspin;
    return i;
}

double Error(double sum, double sum2, int iblk)
{
    if(iblk==1) return 0.0;
    else return sqrt((sum2/(double)iblk - pow(sum/(double)iblk,2))/(double)(iblk-1));
}

/****************************************************************
*****************************************************************
    _/    _/  _/_/_/  _/       Numerical Simulation Laboratory
   _/_/  _/ _/       _/       Physics Department
  _/  _/_/    _/    _/       Universita' degli Studi di Milano
 _/    _/       _/ _/       Prof. D.E. Galli
_/    _/  _/_/_/  _/_/_/_/ email: Davide.Galli@unimi.it
*****************************************************************
*****************************************************************/
