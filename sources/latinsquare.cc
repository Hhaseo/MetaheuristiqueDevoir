/** Le problème du carré latin de base : pas de contrainte d'équilibrage */
#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <algorithm>
using namespace std;
#include <fstream>
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "move.h"
#include "latinsquare.h"

#include "autotuning2.h"

extern ofstream* ofile;  // le fichier de sortie


extern Stat_GWW * Statistiques; // trombe_ajout: l'objet pour les stats en var globale
                         // alloué dans le main() avec npb et nbessai

void arguments_carre(char** argv, int& narg, int & s)
{
  s = argument2ul(argv[narg+1],"nombre de lignes ");
  *ofile <<  "  nb lignes " << s << endl;
  narg++;
}


Latinsquare* latinsquare_problem_creation (int nbvar)
{
  int* dom = new int[nbvar*nbvar];

  vector<int>* tabdom = new vector<int> [nbvar*nbvar] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar*nbvar];

  Latinsquare * problem = probleme_carrelatin(nbvar);

  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;


 return problem;
}



int latinsquare (int argc, char** argv, int tuningmode, int sb) {
  // les divers arguments lus dans la ligne de commande
  int nbvar;
  int taille,nbessais;
  int graine1;
  int narg = 1;  // compteur des arguments

// le nom du fichier de sortie en 1er argument : pour les tests  arg ->  version logiciel + concaténation des arguments
  char filename [1000];
  if ((string)argv[1] == "arg")
    ofile_name(filename, argc, argv);
  else sprintf(filename,"%s",argv[1]);

  ofstream ofile1 (filename);
  ofile = & ofile1;

  // lecture des arguments du problème
  arguments_carre (argv,narg,nbvar);

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

  Latinsquare* problem;
  // creation du probleme (lecture des données, création des structures de données et du problème)
  if (sb) // probleme carré latin  équilibré
    problem = sblatinsquare_problem_creation (nbvar);
  else  // probleme carré latin de base
    problem = latinsquare_problem_creation (nbvar);

  // creation de la population et initialisation
  // La population : tableau de configurations
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->allocate_moves();


  // initialisation des statistiques
  Statistiques->init_pb(0);




// boucle sur les essais
  if (tuningmode)
    autosolving((LSAlgorithm*)algo,population,problem,0,graine1,nbessais,maxtime,1000000);
  else
    {  for(int nessai = 0;nessai< nbessais ; nessai++)
      executer_essai (problem,algo,population,taille,graine1,nessai);

    // ecriture statistiques
    Statistiques->current_try++;
    ecriture_stat_probleme();
    }
  delete problem;
  cout << "Fin résolution " << Statistiques->total_execution_time << endl;
  return 0;

}




Latinsquare::Latinsquare (int nvar) :
CSProblem (nvar*nvar, 1) {domainsize=nvar; squaresize=nvar;}

Latinsquare* probleme_carrelatin (int nbvar)
{Latinsquare* p1 = new Latinsquare (nbvar);
return p1;
}

Configuration* Latinsquare::create_configuration()
{return (new FullincrCSPConfiguration(nbvar,domainsize));}


// evaluation et remplissage de la structure incrementale : le nombre d'occurrences de chaque valeur j dans une colonne i */
int Latinsquare::config_evaluation (Configuration* configuration)
{
 int valeur=0;

 for (int i=0; i< squaresize ; i++)
   {for (int j=0; j< squaresize ; j++)
     *ofile << configuration->config[squaresize*i+j] << " " ;
   *ofile << endl;}

 configuration->init_conflicts();
 for (int i=0; i< squaresize; i++)  // la colonne
   {for (int j=0; j< squaresize; j++)
     configuration->incr_conflicts(i, configuration->config[i+ squaresize*j],configuration->config[i+ squaresize*j],1);}
 for (int i=0; i< squaresize; i++)
   {for (int j=0; j< squaresize; j++)
     if (configuration->get_conflicts(i,j,j)> 1)
       valeur+=
	 configuration->get_conflicts(i,j,j) -1;
   }
 return valeur;
}



// une permutation par ligne
void Latinsquare :: random_configuration(Configuration* configuration)
{for (int i=0;i<squaresize;i++)
  {
    list<int> not_used;
    for (int j=0;j<squaresize ;j++)
      { not_used.push_back(j);}
    for (int j=0;j< squaresize ;j++)
      {int indice = (int) (drand48() * (squaresize -j));
      int jj =0; int val;
      for (list<int>::iterator it = not_used.begin(); it != not_used.end(); it++)
	{if (jj == indice)
	  {val=*it; break;}
	jj++;}
      not_used.remove(val);
      configuration->config[squaresize*i+j]= val;
      }
  }
}

Move* Latinsquare::create_move()
{ColSwMove* move = new ColSwMove();
 return (Move*)move;
}

void Latinsquare::adjust_parameters(Configuration* configuration, int& maxneighbors, int& minneighbors)
{;}


void Latinsquare::next_move
(Configuration* configuration, Move* move, NeighborhoodSearch* nbhs)
{

  //  int line1 = rand () % squaresize;
  int line1= (((ColSwMove*)move)->line +1 ) % squaresize;  //  ligne a tour de role
  int variable1 =  rand () % squaresize;
  // int variable1 = (((ColSwMove*)move)->variable1 +1 ) % squaresize;

  //  cout << "line " << line1 << " variable1 " << variable1 << endl;
  if (nbhs->var_conflict)
    {
     int conf =0; int var=variable1;
    while (variable1 < squaresize)
      {if (
	   ((FullincrCSPConfiguration*)configuration)->tabconflicts[variable1][configuration->config[line1*squaresize+variable1]] > 1)
	{conf=1;break;}
      variable1++;}
    if (!conf)
      {
        variable1=0;
      while (variable1 < var)
	{if (((FullincrCSPConfiguration*)configuration)->tabconflicts[variable1][configuration->config[line1*squaresize+variable1]] > 1) {conf=1;break;}
	variable1++;}
      }
    }

  int variable2 = rand () % (squaresize-1);
  if (variable2 >= variable1)
     variable2++;
  ((ColSwMove*)move)->line= line1;
  ((ColSwMove*)move)->variable1= variable1;
  ((ColSwMove*)move)->variable2= variable2;
  //  cout << "line " << line1 << " variable1 " << variable1 << " variable2 " << variable2 << endl;
 move->valuation = move_evaluation (configuration,move);

}

int Latinsquare::move_evaluation (Configuration* configuration,Move* move)
{
  int line = ((ColSwMove*)move)->line;
  int variable1 = squaresize*line + ((ColSwMove*)move)-> variable1;
  int variable2 = squaresize*line + ((ColSwMove*)move)-> variable2;
  int val1= configuration->config[variable1];
  int val2= configuration->config[variable2];
  int delta =0;
  if (((FullincrCSPConfiguration*)configuration)->tabconflicts[((ColSwMove*)move)-> variable1][val1] > 1 )
    delta--;
  if (((FullincrCSPConfiguration*)configuration)->tabconflicts[((ColSwMove*)move)-> variable1][val2])
    delta++;
  if (((FullincrCSPConfiguration*)configuration)->tabconflicts[((ColSwMove*)move)-> variable2][val2] > 1 )
    delta--;
  if (((FullincrCSPConfiguration*)configuration)->tabconflicts[((ColSwMove*)move)-> variable2][val1])
    delta++;
  return (configuration->valuation + delta);
}


void Latinsquare::fullincr_update_conflicts (FullincrCSPConfiguration* configuration,Move* move)
{
    int line = ((ColSwMove*)move)->line;
    //*ofile << "FINCR " << line << " " << ((ColSwMove*)move)-> variable1 << " " << ((ColSwMove*)move)-> variable2 << endl;
  int variable1 = squaresize*line + ((ColSwMove*)move)-> variable1;
  int variable2 = squaresize*line + ((ColSwMove*)move)-> variable2;
  int val1= configuration->config[variable1];
  int val2= configuration->config[variable2];
  configuration->incr_conflicts( ((ColSwMove*)move)-> variable1,val1,val1,-1);
  configuration->incr_conflicts( ((ColSwMove*)move)-> variable1,val2,val2,1);
  configuration->incr_conflicts( ((ColSwMove*)move)-> variable2,val2,val2,-1);
  configuration->incr_conflicts( ((ColSwMove*)move)-> variable2,val1,val1,1);
      }







void Latinsquare::move_execution(Configuration* configuration,Move* move)
{OpProblem::move_execution(configuration,move);
int line = ((ColSwMove*)move)->line;
  int variable1 = squaresize*line + ((ColSwMove*)move)-> variable1;
  int variable2 = squaresize*line + ((ColSwMove*)move)-> variable2;

 int aval = configuration->config[variable1];

 configuration->config[variable1]=configuration->config[variable2];
 configuration->config[variable2]=aval;
}

//  implante directement dans next_move
void Latinsquare::compute_var_conflict(Configuration* configuration)
{;}




int ColSwMove::eqmove (Move* move1)
{
  return (
	  ((ColSwMove *)move1)->line== line
	  &&    ((ColSwMove *)move1)->variable1== variable1
	   &&  ((ColSwMove*)move1)->variable2 == variable2
	  );
}

/* le mouvement tabou est le mouvement inverse */
Move* ColSwMove::computetabumove(Configuration* configuration)
{ColSwMove* tabumove = new ColSwMove();
 tabumove->line = line;
 tabumove->variable1 = variable2;
 tabumove->variable2 = variable1;
 return tabumove;
}


void ColSwMove::copymove(Move* move1)
{valuation=move1->valuation;
 line=((ColSwMove*)move1)->line;
 variable1=((ColSwMove*)move1)->variable1;
 variable2=((ColSwMove*)move1)->variable2;
}

ColSwMove::ColSwMove() {valuation=0;line=0;variable1=0;variable2=1;}

