#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>

using namespace std;
#include <fstream.h>
#include <string>

#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "queen.h"


#include <math.h>
#include <unistd.h>



extern ofstream* ofile;  // le fichier de sortie

extern Stat_GWW * Statistiques; // trombe_ajout: l'objet pour les stats en var globale pour les signaux
                         // alloué dans le main() avec npb et nbessai

void arguments_reines(char** argv, int& narg, int & s)
{ 
  s = argument2ul(argv[narg+1],"nombre de reines ");
  *ofile << " reines  " << argv[2] << "  nb reines " << s << endl;
  narg++;
}


int main (int argc, char** argv) {

  // les divers arguments lus dans la ligne de commande
  int nbvar;
  int taille,nbessais;
  int graine1;
  int narg = 1;  // compteur des arguments

  // le nom du fichier de sortie : pour les tests : version logiciel + concaténation des arguments
  char filename [1000];
  sprintf(filename,"%s%s","resultats18/",argv[1]);
  for (int i=2;i<argc;i++)
    sprintf(filename,"%s-%s",filename,argv[i]);
  cout << filename << endl;

  ofstream ofile1 (filename);
  ofile = & ofile1;

  // lecture des arguments du problème
  arguments_reines (argv,narg,nbvar);

  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (1, nbessais);
  
  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la récupération du signal 10
  sigaction();

  // creation du probleme (lecture des données, création des structures de données et du problème)
  SwNiNqueen* problem = swniqueen_problem_creation (nbvar);

  // creation de la population et initialisation 
  // La population : tableau de configurations
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->allocate_moves();

  // initialisation des statistiques
  Statistiques->init_pb(0);

  // boucle sur les essais 

  for(int nessai = 0;nessai< nbessais ; nessai++)
    executer_essai (problem,algo,population,taille,graine1,nessai);

  // ecriture statistiques 
  Statistiques->current_try++; 
  ecriture_stat_probleme();
  delete problem;
  cout << "Fin résolution " << Statistiques->total_execution_time << endl;
  return 0;
  
  
}



