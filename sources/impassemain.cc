

#include <iostream>
#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include "timer.h"
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "color.h"


#include <math.h>
#include <unistd.h>

#include <stdlib.h>  // trombe_ajout
#include <signal.h>  // trombe_ajout

extern std::ofstream* ofile;  // le fichier de sortie

extern Stat_GWW * Statistiques; // trombe_ajout: l'objet pour les stats en var globale pour les signaux
                         // alloué dans le main() avec npb et nbessai







void arguments_impasse(char** argv, int& narg, int & degre)
{ degre = argument2bul(argv[narg+1]," indicateur somme degrés ",0,1 );
  *ofile << " somme degrés " << degre << std::endl;
  narg= narg+1;
 }

int main (int argc, char** argv) {

  // les divers arguments lus dans la ligne de commande

  int nbcol,degre,taille,nbessais;
  int graine1;
  int narg = 1;  // compteur des arguments

  // le nom du fichier de sortie : pour les tests : version logiciel + concaténation des arguments
  char filename [1000];
  if ((std::string)argv[1] == "arg")
    ofile_name(filename, argc, argv);
  else sprintf(filename,"%s",argv[1]);

  std::ofstream ofile1 (filename);
  ofile = & ofile1;

  std::ifstream file (argv[1]); // le fichier de données

  // lecture des arguments du problème
  arguments_coloriage(argv,narg,nbcol);
  arguments_impasse(argv,narg,degre);
  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (1, nbessais);

  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la récupération du signal 10
  sigaction();

  // creation du probleme (lecture des données, création des structures de données et du problème)
  ImpasseColor* problem = impasse_problem_creation (nbcol,degre,file);

  // creation de la population et initialisation
  // La population : tableau de configurations
  problem->domainsize++; // pour la valeur supplementaire soit dans la structure des conflits
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->domainsize--; // pour enlever la valeur supplementaire
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
  std::cout << "Fin résolution " << Statistiques->total_execution_time << std::endl;
  return 0;


}



