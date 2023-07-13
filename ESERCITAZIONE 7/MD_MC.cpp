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
#include <iomanip>
#include <fstream>
#include <ostream>
#include <cmath>

#include "MD_MC.h"

using namespace std;

int main()
{ 
  Input(); //Inizialization
  int nconf = 1;

  for(int iblk=1; iblk <= nblk; iblk++) //Simulation
  {
    Reset(iblk);   //Reset block averages
    for(int istep=1; istep <= nstep; istep++)
    {
      Move();
      Measure();
      Accumulate(); //Update block averages
      if(istep%10 == 0){
//        ConfXYZ(nconf);//Write actual configuration in XYZ format //Commented to avoid "filesystem full"! 
        nconf += 1;
      }
    }
    Averages(iblk);   //Print results for current block
  }
  ConfFinal(); //Write final configuration

  return 0;
}



void Input(void)
{
  ifstream ReadInput, ReadConf, ReadVelocity, Primes, Seed;

  cout << "Classic Lennard-Jones fluid        " << endl;
  cout << "MD(NVE) / MC(NVT) simulation       " << endl << endl;
  cout << "Interatomic potential v(r) = 4 * [(1/r)^12 - (1/r)^6]" << endl << endl;
  cout << "Boltzmann weight exp(- beta * sum_{i<j} v(r_ij) ), beta = 1/T " << endl << endl;
  cout << "The program uses Lennard-Jones units " << endl;

//Read seed for random numbers
  int p1, p2;
  Primes.open("../RANDOM_GEN/Primes");
  if (Primes.is_open()){
    Primes >> p1 >> p2 ;
  } else cerr << "PROBLEM: Unable to open Primes" << endl;
  Primes.close();

//Read input informations
  ReadInput.open("input.in");

  ReadInput >> iNVET;
  ReadInput >> restart;

  if(restart) Seed.open("seed.out");
  else Seed.open("../RANDOM_GEN/seed.in");
  Seed >> seed[0] >> seed[1] >> seed[2] >> seed[3];
  rnd.SetRandom(seed,p1,p2);
  Seed.close();

  ReadInput >> temp;
  v_beta = 1.0/temp;
  cout << "Temperature = " << temp << endl;

  ReadInput >> npart;
  cout << "Number of particles = " << npart << endl;

  ReadInput >> rho;
  cout << "Density of particles = " << rho << endl;
  vol = (double)npart/rho;
  box = pow(vol,1.0/3.0);
  cout << "Volume of the simulation box = " << vol << endl;
  cout << "Edge of the simulation box = " << box << endl;

  ReadInput >> rcut;
  cout << "Cutoff of the interatomic potential = " << rcut << endl << endl;
    
  ReadInput >> delta;

  ReadInput >> nblk;

  ReadInput >> nstep;

  ReadInput >> nbins;
  bin_size = box/(2 * nbins);

  //Quanti step fare per la termalizzazione
  ReadInput >> term;  

  cout << "The program perform Metropolis moves with uniform translations" << endl;
  cout << "Moves parameter = " << delta << endl;
  cout << "Number of blocks = " << nblk << endl;
  cout << "Number of steps in one block = " << nstep << endl;
  cout << "Number of bins = " << nbins << endl;
  cout << "Bin size = " << bin_size << endl;

  ReadInput.close();

//Prepare arrays for measurements
  iv = 0; //Potential energy
  it = 1; //Temperature
  ik = 2; //Kinetic energy
  ie = 3; //Total energy
  iw = 4; //Total pressure
  n_props = 5 + nbins; //Number of observables

//Read initial configuration
  cout << "Read initial configuration" << endl << endl;
  if(restart)
  {
    ReadConf.open("config.out");
    ReadVelocity.open("velocity.out");
    for (int i=0; i<npart; ++i) ReadVelocity >> vx[i] >> vy[i] >> vz[i];
  }
  else 
  {
    ReadConf.open("config.in");
    cout << "Prepare velocities with center of mass velocity equal to zero " << endl;
    double sumv[3] = {0.0, 0.0, 0.0};
    for (int i=0; i<npart; ++i)
    {
      vx[i] = rnd.Gauss(0.,sqrt(temp));
      vy[i] = rnd.Gauss(0.,sqrt(temp));
      vz[i] = rnd.Gauss(0.,sqrt(temp));
      sumv[0] += vx[i];
      sumv[1] += vy[i];
      sumv[2] += vz[i];
    }
    for (int idim=0; idim<3; ++idim) sumv[idim] /= (double)npart;
    double sumv2 = 0.0, fs;
    for (int i=0; i<npart; ++i)
    {
      vx[i] = vx[i] - sumv[0];
      vy[i] = vy[i] - sumv[1];
      vz[i] = vz[i] - sumv[2];
      sumv2 += vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i];
    }
    sumv2 /= (double)npart;
    fs = sqrt(3 * temp / sumv2);   // fs = velocity scale factor 
    cout << "velocity scale factor: " << fs << endl << endl;
    for (int i=0; i<npart; ++i)
    {
      vx[i] *= fs;
      vy[i] *= fs;
      vz[i] *= fs;
    }
  }

  for (int i=0; i<npart; ++i)
  {
    ReadConf >> x[i] >> y[i] >> z[i];
    x[i] = Pbc( x[i] * box );
    y[i] = Pbc( y[i] * box );
    z[i] = Pbc( z[i] * box );
  }
  ReadConf.close();

  for (int i=0; i<npart; ++i)
  {
    if(iNVET)
    {
      xold[i] = x[i];
      yold[i] = y[i];
      zold[i] = z[i];
    }
    else
    {
      //Calcolo posizioni in maniera approssimata
      xold[i] = Pbc(x[i] - vx[i] * delta);
      yold[i] = Pbc(y[i] - vy[i] * delta);
      zold[i] = Pbc(z[i] - vz[i] * delta);
    }
  }

//Evoluzione per termalizzazione
  for(int i=0; i<term; i++) { Move(); }
  
//Evaluate properties of the initial configuration
  Measure();

//Print initial values for measured properties
  cout << "Initial potential energy = " << walker[iv]/(double)npart + 8 * pi * rho * (1/(9 * pow(rcut, 9)) - 1/(3 * pow(rcut, 3))) << endl;
  cout << "Initial temperature      = " << walker[it] << endl;
  cout << "Initial kinetic energy   = " << walker[ik]/(double)npart << endl;
  cout << "Initial total energy     = " << walker[ie]/(double)npart + 8 * pi * rho * (1/(9 * pow(rcut, 9)) - 1/(3 * pow(rcut, 3))) << endl;

  cout << "----------------------------" << endl << endl;

  return;
}



void Move()
{
  int o;
  double p, energy_old, energy_new;
  double xnew, ynew, znew;

  if(iNVET) // Monte Carlo (NVT) move
  {
    for(int i=0; i<npart; ++i)
    {
    //Select randomly a particle (for C++ syntax, 0 <= o <= npart-1)
      o = (int)(rnd.Rannyu()*npart);

    //Old
      energy_old = Boltzmann(x[o],y[o],z[o],o);

    //New
      x[o] = Pbc( x[o] + delta*(rnd.Rannyu() - 0.5) );
      y[o] = Pbc( y[o] + delta*(rnd.Rannyu() - 0.5) );
      z[o] = Pbc( z[o] + delta*(rnd.Rannyu() - 0.5) );

      energy_new = Boltzmann(x[o],y[o],z[o],o);

    //Metropolis test
      p = exp(v_beta*(energy_old-energy_new));
      if(p >= rnd.Rannyu())  
      {
      //Update
        xold[o] = x[o];
        yold[o] = y[o];
        zold[o] = z[o];
        accepted = accepted + 1.0;
      } else {
        x[o] = xold[o];
        y[o] = yold[o];
        z[o] = zold[o];
      }
      attempted = attempted + 1.0;
    }
  } else // Molecular Dynamics (NVE) move
  {
    double fx[m_part], fy[m_part], fz[m_part];

    for(int i=0; i<npart; ++i){ //Force acting on particle i
      fx[i] = Force(i,0);
      fy[i] = Force(i,1);
      fz[i] = Force(i,2);
    }

    for(int i=0; i<npart; ++i){ //Verlet integration scheme

      xnew = Pbc( 2.0 * x[i] - xold[i] + fx[i] * pow(delta,2) );
      ynew = Pbc( 2.0 * y[i] - yold[i] + fy[i] * pow(delta,2) );
      znew = Pbc( 2.0 * z[i] - zold[i] + fz[i] * pow(delta,2) );

      vx[i] = Pbc(xnew - xold[i])/(2.0 * delta);
      vy[i] = Pbc(ynew - yold[i])/(2.0 * delta);
      vz[i] = Pbc(znew - zold[i])/(2.0 * delta);

      xold[i] = x[i];
      yold[i] = y[i];
      zold[i] = z[i];

      x[i] = xnew;
      y[i] = ynew;
      z[i] = znew;

      accepted = accepted + 1.0;
      attempted = attempted + 1.0;
    }
  }
  return;
}



double Boltzmann(double xx, double yy, double zz, int ip)
{
  double ene=0.0;
  double dx, dy, dz, dr;

  for (int i=0; i<npart; ++i)
  {
    if(i != ip)
    {
// distance ip-i in pbc
      dx = Pbc(xx - x[i]);
      dy = Pbc(yy - y[i]);
      dz = Pbc(zz - z[i]);

      dr = dx*dx + dy*dy + dz*dz;
      dr = sqrt(dr);

      if(dr < rcut)
      {
        ene += 1.0/pow(dr,12) - 1.0/pow(dr,6);
      }
    }
  }

  return 4.0*ene;
}



double Force(int ip, int idir){ //Compute forces as -Grad_ip V(r)
  double f=0.0;
  double dvec[3], dr;

  for (int i=0; i<npart; ++i){
    if(i != ip){
      dvec[0] = Pbc( x[ip] - x[i] );  // distance ip-i in pbc
      dvec[1] = Pbc( y[ip] - y[i] );
      dvec[2] = Pbc( z[ip] - z[i] );

      dr = dvec[0]*dvec[0] + dvec[1]*dvec[1] + dvec[2]*dvec[2];
      dr = sqrt(dr);

      if(dr < rcut){
        f += dvec[idir] * (48.0/pow(dr,14) - 24.0/pow(dr,8)); // -Grad_ip V(r)
      }
    }
  }
 
  return f;
}



void Measure() //Properties measurement
{
  double v = 0.0, kin=0.0;
  double vij; double p = 0.0;
  double dx, dy, dz, dr;

//Necesario per normalizzazione della g(r) (se no sommo cose che non dovrei sommare)
  for(int i=5; i<n_props; i++) { walker[i] = 0; }

//cycle over pairs of particles
  for (int i=0; i<npart-1; ++i)
  {
    for (int j=i+1; j<npart; ++j)
    {
      // distance i-j in pbc
      dx = Pbc(x[i] - x[j]);
      dy = Pbc(y[i] - y[j]);
      dz = Pbc(z[i] - z[j]);

      dr = dx*dx + dy*dy + dz*dz;
      dr = sqrt(dr);

      //Calcolo per la g(r) (ho già inserito la normalizzazione)
      sd = 5 + floor(dr/bin_size);
      walker[(int)sd] += 6/(4 * pi * rho * npart * (pow((sd - 4) * bin_size, 3) - pow((sd - 5) * bin_size, 3)));

      if(dr < rcut)
      {
        vij = 1.0/pow(dr,12) - 1.0/pow(dr,6);
        v += vij;                     //Calcolo per il potenziale
        p += vij + 0.5/pow(dr, 6);    //Calcolo per la pressione
      }
    }          
  }

  //Calcolo per l'energia cinetica
  for (int i=0; i<npart; ++i) kin += 0.5 * (vx[i]*vx[i] + vy[i]*vy[i] + vz[i]*vz[i]);

  walker[iv] = 4.0 * v; // Potential energy
  walker[ik] = kin; // Kinetic energy
  walker[it] = (2.0 / 3.0) * kin/(double)npart; // Temperature
  walker[ie] = 4.0 * v + kin;  // Total energy;
  walker[iw] = 16 * p; //Pressure

  return;
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

   //Calcolo tail correction per energia e pressione
   vtail = 8 * pi * rho * (1/(9 * pow(rcut, 9)) - 1/(3 * pow(rcut, 3)));
   ptail = 32 * pi * rho * (1/(9 * pow(rcut, 9)) - 1/(6 * pow(rcut, 3))); 
    
   ofstream Epot, Ekin, Etot, Temp, Press;
   const int wd=12;
    
    cout << "Block number " << iblk << endl;
    cout << "Acceptance rate " << accepted/attempted << endl << endl;
    
    Epot.open("output_epot.dat",ios::app);
    Ekin.open("output_ekin.dat",ios::app);
    Temp.open("output_temp.dat",ios::app);
    Etot.open("output_etot.dat",ios::app);
    Press.open("output_press.dat",ios::app);


    /*********************************************
    *             Energia potenziale             *
    *********************************************/
    stima_pot = blk_av[iv]/blk_norm/(double)npart + vtail;
    glob_av[iv] += stima_pot;
    glob_av2[iv] += stima_pot*stima_pot;
    err_pot = Error(glob_av[iv],glob_av2[iv],iblk);
    


    /**********************************************
    *               Energia cinetica              *
    **********************************************/
    stima_kin = blk_av[ik]/blk_norm/(double)npart; 
    glob_av[ik] += stima_kin;
    glob_av2[ik] += stima_kin*stima_kin;
    err_kin = Error(glob_av[ik],glob_av2[ik],iblk);



    /**********************************************
    *                Energia totale               *
    **********************************************/
    stima_etot = blk_av[ie]/blk_norm/(double)npart + vtail;
    glob_av[ie] += stima_etot;
    glob_av2[ie] += stima_etot*stima_etot;
    err_etot = Error(glob_av[ie],glob_av2[ie],iblk);



    /**********************************************
    *                 Temperatura                 *
    **********************************************/
    stima_temp = blk_av[it]/blk_norm;
    glob_av[it] += stima_temp;
    glob_av2[it] += stima_temp*stima_temp;
    err_temp = Error(glob_av[it],glob_av2[it],iblk);



    /***********************************************
    *                  Pressione                   *
    ***********************************************/
    stima_press = rho * stima_temp + (blk_av[iw]/(double)blk_norm + ptail * (double)npart)/vol;
    glob_av[iw] += stima_press;
    glob_av2[iw] += stima_press*stima_press;
    err_press = Error(glob_av[iw], glob_av2[iw], iblk);



    /***********************************************
    *                     g(r)                     *
    ***********************************************/
    for(int i=0; i<nbins; i++) {
      glob_av[5 + i] += blk_av[5 + i]/blk_norm;
      glob_av2[5 + i] += blk_av[5 + i] * blk_av[5 + i]/(blk_norm * blk_norm);
    }

//Potential energy per particle
    Epot << setw(wd) << iblk <<  setw(wd) << stima_pot << setw(wd) << glob_av[iv]/(double)iblk << setw(wd) << err_pot << endl;
//Kinetic energy
    Ekin << setw(wd) << iblk <<  setw(wd) << stima_kin << setw(wd) << glob_av[ik]/(double)iblk << setw(wd) << err_kin << endl;
//Total energy
    Etot << setw(wd) << iblk <<  setw(wd) << stima_etot << setw(wd) << glob_av[ie]/(double)iblk << setw(wd) << err_etot << endl;
//Temperature
    Temp << setw(wd) << iblk <<  setw(wd) << stima_temp << setw(wd) << glob_av[it]/(double)iblk << setw(wd) << err_temp << endl;
//Pressure
    Press << setw(wd) << iblk <<  setw(wd) << stima_press << setw(wd) << glob_av[iw]/(double)iblk << setw(wd) << err_press << endl;


    cout << "----------------------------" << endl << endl;

    Epot.close();
    Ekin.close();
    Etot.close();
    Temp.close();
    Press.close();
}


void ConfFinal(void)
{
  ofstream g_r;
  ofstream WriteConf, WriteVelocity, WriteSeed;

  cout << "Print final configuration to file config.out" << endl << endl;
  WriteConf.open("config.out");
  WriteVelocity.open("velocity.out");
  for (int i=0; i<npart; ++i)
  {
    WriteConf << x[i]/box << "   " <<  y[i]/box << "   " << z[i]/box << endl;
    WriteVelocity << vx[i] << "   " <<  vy[i] << "   " << vz[i] << endl;
  }
  WriteConf.close();
  WriteVelocity.close();

  rnd.SaveSeed();



  /*************************************
  *            Stampo g(r)             *
  *************************************/
  g_r.open("output.g_r.dat", ios::app);

  //Stampo valori per la g(r)
  for(int i=0; i<nbins; i++) {
    err_gdir = Error(glob_av[5 + i], glob_av2[5 + i], nblk);
    g_r << i+1 << setw(12) << glob_av[5 + i]/(double)nblk << setw(12) << err_gdir << endl;
  }

  g_r.close();


}

void ConfXYZ(int nconf){ //Write configuration in .xyz format
  ofstream WriteXYZ;

  WriteXYZ.open("frames/config_" + to_string(nconf) + ".xyz");
  WriteXYZ << npart << endl;
  WriteXYZ << "This is only a comment!" << endl;
  for (int i=0; i<npart; ++i){
    WriteXYZ << "LJ  " << Pbc(x[i]) << "   " <<  Pbc(y[i]) << "   " << Pbc(z[i]) << endl;
  }
  WriteXYZ.close();
}

double Pbc(double r)  //Algorithm for periodic boundary conditions with side L=box
{
    return r - box * rint(r/box);
}

double Error(double sum, double sum2, int iblk)
{
    return sqrt(fabs(sum2/(double)iblk - pow(sum/(double)iblk,2))/(double)iblk);
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