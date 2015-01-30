#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>
#include <fstream>
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "maxclique.h"
#include "color.h"
#include "move.h"
#include "autotuning2.h"

extern std::ofstream* ofile;  // le fichier de sortie
extern Stat_GWW * Statistiques;

void arguments_clique(char** argv, int& narg, int & clsize)
{
  clsize = argument2ul(argv[narg+1],"taille clique ");
  *ofile << " maxclique  " << argv[narg+1] << "  taille clique " << clsize << std::endl;
  narg++;
}


/** Tout ce qui concerne le probleme max-clique :
on essaie de trouver une clique de taille n donnée en minimisant les conflits (aretes manquantes
dans l'ensemble des n sommets candidats
Un mouvement est un échange entre un sommet dans la clique candidate participant à un conflit
et un sommet hors clique (remainvariables)
Utilisation de ExchangeMove défini  dans move
*/

/* meme format de fichier DIMACS que pour le coloriage  */
void lire_debut_fichier_clique(std::ifstream & file, int& nbvar, int& nbconst)
{lire_debut_fichier_coloriage (file,  nbvar,  nbconst);}

void lire_fichier_clique (std::ifstream& file, std::vector<int>* connexions,int ** constraint1)
{lire_fichier_coloriage (file,  connexions, constraint1);}


CliqueProblem* clique_problem_creation (int clsize,std::ifstream & file)
{

  // Declaration des variables contenant les structures de données des problemes
  int **constraint1; // utilise dans tout csp binaire
  int nbvar; int nbconst;
  lire_debut_fichier_clique(file,nbvar,nbconst); // pour dimensionner les structures

  constraint1 =  csp_constraintdatastructure(nbvar);

  int* dom = new int[nbvar];
  std::vector<int>* tabdom = new std::vector<int> [nbvar] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  std::vector<int>* connex = new std::vector<int> [nbvar];

  lire_fichier_clique(file,connex,constraint1);
  CliqueProblem * problem = probleme_maxclique(nbvar,nbconst,clsize,constraint1);
  // mise en place des domaines et connexions
  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;
}


CliqueProblem::CliqueProblem (int nvar, int nconst,int clsize) :
BinaryCSProblem (nvar,nconst){domainsize=2;cliquesize=clsize;}

CliqueProblem* probleme_maxclique(int nbvar,int nbconst,int clsize,int** constraint1)
{CliqueProblem* p1 = new CliqueProblem(nbvar,nbconst,clsize);
p1->constraints=constraint1;
return p1;
}


int maxclique (int argc, char** argv, int tuningmode) {

  // les divers arguments lus dans la ligne de commande

  int clsize,taille,nbessais;
  int graine1;
  int narg = 2;  // compteur des arguments

  // le nom du fichier de sortie : pour les tests : version logiciel + concaténation des arguments
  char filename [1000];
  if ((std::string)argv[1] == "arg")
    ofile_name(filename, argc, argv);
  else sprintf(filename,"%s",argv[1]);

  std::ofstream ofile1 (filename);
    ofile = & ofile1;

  std::ifstream file (argv[2]); // le fichier de données

  // lecture des arguments du problème
  arguments_clique(argv,narg,clsize);

  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (1, nbessais);

  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la récupération du signal 10
  sigaction();
// argument de temps maximum
  double maxtime;
  if (tuningmode)  arguments_tempscpu (argv,narg,maxtime);

  // creation du probleme (lecture des données, création des structures de données et du problème)
  CliqueProblem* problem = clique_problem_creation (clsize,file);

  // creation de la population et initialisation
  // La population : tableau de configurations
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->allocate_moves();

  // initialisation des statistiques
  Statistiques->init_pb(0);

  // boucle sur les essais
  if (tuningmode)
    autosolving((LSAlgorithm*)algo,population,problem,0,graine1,nbessais,maxtime,1000);
  else
    {  // boucle sur les essais

      for(int nessai = 0;nessai< nbessais ; nessai++)
	executer_essai (problem,algo,population,taille,graine1,nessai);

      // ecriture statistiques
      Statistiques->current_try++;
    }
  delete problem;
  std::cout << "Fin résolution " << Statistiques->total_execution_time << std::endl;
  return 0;


}





int* CliqueProblem::clique(Configuration* configuration)
{return ((CliqueConfiguration*)configuration)->clique;}

int* CliqueProblem::remainvariables(Configuration* configuration)
{return ((CliqueConfiguration*)configuration)->remainvariables;}


int CliqueProblem::config_evaluation (Configuration* configuration)
{int valeur=0;
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   {int cliquemember=0;
   for(int j=0; j< cliquesize; j++)
    {if (clique(configuration)[j]==i)
      {cliquemember=1; break;}
    }
   for(int j=0; j< cliquesize; j++)
     {if ((i <clique(configuration)[j]) &&
	  constraints[i][clique(configuration)[j]]==0 ||
	  (!cliquemember && i > clique(configuration)[j] && constraints[clique(configuration)[j]][i]==0))
       {if (cliquemember) {valeur++; }
       configuration->incr_conflicts(i,1,1,1);
       if (cliquemember) configuration->incr_conflicts(clique(configuration)[j],1,1,1);}
     }
   }


 return valeur;
}

int CliqueProblem::config_evaluation_verif (Configuration* configuration)
{int valeur=0;
 for(int i=0; i<nbvar; i++)
   {int cliquemember=0;
   for(int j=0; j< cliquesize; j++)
    {if (clique(configuration)[j]==i)
      {cliquemember=1; break;}
    }
   for(int j=0; j< cliquesize; j++)
     {if ((i <clique(configuration)[j]) &&
	  constraints[i][clique(configuration)[j]]==0 ||
	  (!cliquemember && i > clique(configuration)[j] && constraints[i][clique(configuration)[j]]==0))
       {if (cliquemember) {valeur++; }
       }
     }
   }
 return valeur;
}



void CliqueProblem::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)
{int var_out = clique(configuration)[((ExchangeMove*)move)-> variable1];
   int var_in = remainvariables(configuration)[((ExchangeMove*)move)-> variable2];

  for (int j=0; j< nbvar; j++)
    {if (((j > var_out) && constraints[var_out][j]==0) ||
	 ((j < var_out) && constraints[j][var_out]==0))
      {
	configuration->tabconflicts[j][1]--;
      }
    if (((j > var_in) && constraints[var_in][j]==0) ||
	((j < var_in) && constraints[j][var_in]==0))
      {
	configuration->tabconflicts[j][1]++;
      }
    }

}




void CliqueProblem::move_execution(Configuration* configuration,Move* move)
{ OpProblem::move_execution(configuration,move);
  int var_out = clique(configuration)[((ExchangeMove*)move)-> variable1];
  int var_in = remainvariables(configuration)[((ExchangeMove*)move)-> variable2];
  configuration->config[var_in]=1;
  configuration->config[var_out]=0;
  ((CliqueConfiguration*)configuration)->clique[((ExchangeMove*)move)-> variable1]=var_in;
  ((CliqueConfiguration*)configuration)->remainvariables[((ExchangeMove*)move)-> variable2]=var_out;
  //   int value1 = config_evaluation_verif(configuration);
  //      if (value1 != move->valuation)
  //           *ofile << "erreur " << value1 << " " << move->valuation << endl;
}

/* non implanté */
void CliqueProblem::incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move)
{;}


Move* CliqueProblem::create_move()
{ExchangeMove* move = new ExchangeMove();
 return (Move*)move;
}



void CliqueProblem::next_move
(Configuration* configuration, Move* move, NeighborhoodSearch* nbhs)
{int conflictvariable = 0; int index1;
  while(!conflictvariable)
    {index1 =  (int) (drand48() * cliquesize);
    if (((FullincrCSPConfiguration*)configuration)->tabconflicts[clique(configuration)[index1]][1])
      conflictvariable=1;
    }
  ((ExchangeMove*)move)->variable1 = index1;
  if (nbhs->val_conflict)
    ((ExchangeMove*)move)->variable2 = minconflict_invariable(configuration);
  else
    ((ExchangeMove*)move)->variable2 = (int) (drand48 () * (nbvar - cliquesize));

  move->valuation = move_evaluation (configuration,move);
}

int CliqueProblem::minconflict_invariable(Configuration* configuration)
{ int minpromises=RAND_MAX; int promises=0; int k1=1; int j=0;
   for (int i =0; i< nbvar-cliquesize; i++)
     {
     promises= ((CliqueConfiguration*)configuration)->tabconflicts[remainvariables(configuration)[i]][1];
     if (promises < minpromises)
       {minpromises = promises; j=i; k1=1;}
     else if (promises == minpromises)
       {k1++;
       if (drand48() < 1/(double)k1) j=i;
       }
     }
   return j;
}


int CliqueProblem::move_evaluation (Configuration* configuration, Move* move)
{
  int var_out = clique(configuration)[((ExchangeMove*)move)-> variable1];
  int var_in = remainvariables(configuration)[((ExchangeMove*)move)-> variable2];
  int valeur = configuration->valuation +
  ((FullincrCSPConfiguration*)configuration)->tabconflicts[var_in][1]-
  ((FullincrCSPConfiguration*)configuration)->tabconflicts[var_out][1];
  if (((var_out > var_in) && constraints[var_in][var_out] ==0) ||
      ((var_in > var_out) && constraints[var_out][var_in] ==0))
    valeur--;
  //      *ofile << " conflits var in " << ((FullincrCSPConfiguration*)configuration)->tabconflicts[var_in][1] << " conflits var out " << ((FullincrCSPConfiguration*)configuration)->tabconflicts[var_out][1] << " valeur mvt " << valeur << endl;
  return valeur;
}

void CliqueProblem::random_configuration(Configuration* configuration)
{
  for (int j=0; j<cliquesize; j++)
    clique(configuration)[j]=nbvar;
  for (int i=0; i<nbvar; i++)
    configuration->config[i]=0;
  for(int j=0;j<cliquesize;j++)
    {  int var2 = (int) (drand48 () * (nbvar - j));
    int index=0;
    for (int k=0;k<nbvar;k++)
      {if (configuration->config[k]==0)
	index++;
      if (index > var2) {((CliqueConfiguration*)configuration)->clique[j]=k; configuration->config[k]=1; break;}
      }
    }
  int k=0;
  for(int i=0;i<nbvar;i++)
    if (configuration->config[i]==0)
      {((CliqueConfiguration*)configuration)->remainvariables[k]=i;k++;}
  //  *ofile << " clique courante " ;
  //  for (int j=0; j< cliquesize; j++)
  //    *ofile << clique(configuration)[j] << " " ;
  //  *ofile << endl;
}

void CliqueProblem::solution_write()
{ *ofile<< " meilleure clique" << std::endl;
for (int j=0; j< cliquesize; j++)
    *ofile << ((CliqueConfiguration*)best_config)->clique[j] << " " ;
  *ofile << std::endl;
}

void CliqueProblem::adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors)
    {
    if 	(maxneighbors > (int) (configuration->var_conflict.size()) * (nbvar - cliquesize))
      maxneighbors = configuration->var_conflict.size() * (domainsize -1);
    if 	(minneighbors > (int) (configuration->var_conflict.size()) * (nbvar - cliquesize))
      minneighbors = configuration->var_conflict.size() * (domainsize-1);
    }



Configuration* CliqueProblem::create_configuration ()
{return (new  CliqueConfiguration (nbvar,cliquesize));
}

CliqueConfiguration::CliqueConfiguration(int nbv, int cliques) : FullincrCSPConfiguration(nbv,2)
{cliquesize=cliques;
 clique = new int[cliquesize];
 remainvariables = new int[nbv-cliquesize];
}

CliqueConfiguration::~CliqueConfiguration()
{delete clique; delete remainvariables;}

void CliqueConfiguration::copy_element (Configuration* config2)
{FullincrCSPConfiguration::copy_element(config2);
for (int i=0; i<cliquesize ;i++)
  clique[i]=((CliqueConfiguration*)config2)->clique[i];
for (int i=0; i<nbvar- cliquesize ;i++)
  remainvariables[i]=((CliqueConfiguration*)config2)->remainvariables[i];
}

