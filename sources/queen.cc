#include <list>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>
using namespace std;
#include <fstream>
#include "incop.h"
#include "csproblem.h"
#include "queen.h"
#include "move.h"

extern ofstream* ofile;  // le fichier de sortie

/* le probleme des N-reines : 3 implantations
Nqueen : CSP classique - mouvement changement valeur d'une variable
SWNqueen : mouvement de type SwapMove : permutation des valeurs de 2 variables en tout incrémental
SWNiNqueen : mouvement de type SwapMove :  pas d'incrementalité : version la plus rapide
*/

Nqueen* queen_problem_creation (int nbvar)
{
  int* dom = new int[nbvar];

  vector<int>* tabdom = new vector<int> [nbvar] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar];

  Nqueen * problem = probleme_reines(nbvar);

  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;


 return problem;
}

void Nqueen::init_domaines(int nbvar, int s)
{for (int i=0;i<nbvar;i++)
  domains[i]=0;
}


Nqueen::Nqueen (int nvar) :
BinaryCSProblem (nvar,(nvar*(nvar-1))/2){domainsize=nvar;}

Nqueen* probleme_reines(int nbvar)
{Nqueen* p1 = new Nqueen(nbvar);
return p1;
}

Configuration* Nqueen::create_configuration()
{return (new FullincrCSPConfiguration(nbvar,domainsize));}

int Nqueen::config_evaluation (Configuration* configuration)
{int valeur=0;
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   for(int j=i+1; j<nbvar;j++)
       {if ((configuration->config[i]==configuration->config[j]) ||
	    (configuration->config[i]-configuration->config[j]== j-i)||
            (configuration->config[j]-configuration->config[i]== j-i))
	 {valeur++;}
       configuration->incr_conflicts(j,configuration->config[i],configuration->config[i],1);
       configuration->incr_conflicts(i,configuration->config[j],configuration->config[j],1);
       if (configuration->config[i] + j- i < nbvar)
	  configuration->incr_conflicts(j,configuration->config[i] + j -i,configuration->config[i] + j -i,1);
       if (configuration->config[i] -( j- i) >=0)
	  configuration->incr_conflicts(j,configuration->config[i] - ( j -i),configuration->config[i] - ( j -i),1);
       if (configuration->config[j] + j- i < nbvar)
	 configuration->incr_conflicts(i,configuration->config[j] + j -i,configuration->config[j] + j -i,1);
       if (configuration->config[j] - (j- i) >= 0)
	 configuration->incr_conflicts(i,configuration->config[j] - (j -i),configuration->config[j] - (j -i),1);
       }
 /* for (int i =0 ; i< nbvar ; i++)
   *ofile << configuration.config[i] << " " ;
   *ofile << endl;*/
 return valeur;

}


void Nqueen::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)

{int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  for (int i=0; i< var_changee; i++)

   {
     if (configuration->config[var_changee]-(var_changee-i) >= 0)
       configuration->incr_conflicts(i,configuration->config[var_changee]-(var_changee -i),configuration->config[var_changee]-(var_changee -i),-1);
     if (val_changee-(var_changee-i) >= 0)
       configuration->incr_conflicts(i,val_changee-(var_changee -i),val_changee-(var_changee -i),1);
     if (configuration->config[var_changee]+(var_changee-i) < nbvar)
       configuration->incr_conflicts(i,configuration->config[var_changee]+(var_changee -i),configuration->config[var_changee]+(var_changee -i),-1);
     if (val_changee+(var_changee-i) < nbvar)
       configuration->incr_conflicts(i,val_changee+(var_changee -i),val_changee+(var_changee -i),1);
     configuration->incr_conflicts(i,configuration->config[var_changee],configuration->config[var_changee],-1);
     configuration->incr_conflicts(i,val_changee,val_changee,1);
   }
for (int i=var_changee+1; i< nbvar; i++)
{
   if (configuration->config[var_changee]-(i-var_changee) >= 0)
       configuration->incr_conflicts(i,configuration->config[var_changee]-(i-var_changee),configuration->config[var_changee]-(i-var_changee),-1);
     if (val_changee-(i-var_changee) >= 0)
       configuration->incr_conflicts(i,val_changee-(i-var_changee),val_changee-(i-var_changee),1);
     if (configuration->config[var_changee]+(i-var_changee) < nbvar)
       configuration->incr_conflicts(i,configuration->config[var_changee]+(i-var_changee),configuration->config[var_changee]+(i-var_changee),-1);
     if (val_changee+(i-var_changee) < nbvar)
       configuration->incr_conflicts(i,val_changee+(i-var_changee),val_changee+(i-var_changee),1);
     configuration->incr_conflicts(i,configuration->config[var_changee],configuration->config[var_changee],-1);
     configuration->incr_conflicts(i,val_changee,val_changee,1);
   }
}


/* pour les mouvements en swap  avec tabconflicts complete */



SwNqueen* swqueen_problem_creation (int nbvar)
{
  int* dom = new int[nbvar];

  vector<int>* tabdom = new vector<int> [nbvar] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar];

  SwNqueen * problem = probleme_swreines(nbvar);

  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;
}



SwNqueen::SwNqueen (int numvar) : Nqueen(numvar) {};


SwNqueen* probleme_swreines(int nbvar)
{SwNqueen* p1 = new SwNqueen(nbvar);
return p1;
}

void SwNqueen :: random_configuration(Configuration* configuration)
{list<int> not_used;
 for (int j=0;j<nbvar;j++)
   { not_used.push_back(j);}
 for (int j=0;j<nbvar;j++)
 {int indice = (int) (drand48() * (nbvar -j));
 int jj =0; int val;
 for (list<int>::iterator it = not_used.begin(); it != not_used.end(); it++)
   {if (jj == indice)
     {val=*it; break;}
   jj++;}
 not_used.remove(val);
 configuration->config[j]= index2value(val,j);
 }
}

Move* SwNqueen::create_move()
{SwapMove* move = new SwapMove();
 return (Move*)move;
}

void SwNqueen::next_move
(Configuration* configuration, Move* move, NeighborhoodSearch* nbhs)
{
 if (nbhs->var_conflict)
    ((SwapMove*)move)->variable1 = random_conflict_variable(configuration);
 else
   ((SwapMove*)move)->variable1 = random_variable(configuration);
 int var2 = (int) (drand48 () * (nbvar -1));
 if (var2 >= ((SwapMove*)move)->variable1)
   var2++;
 ((SwapMove*)move)->variable2 = var2;
 move->valuation = move_evaluation (configuration,move);

}

// cas full incremental
int SwNqueen::move_evaluation (Configuration* configuration,Move* move)
{
  int variable1 = ((SwapMove*)move)-> variable1;
  int variable2 = ((SwapMove*)move)-> variable2;
  int valeur = configuration->valuation +
    configuration->get_conflicts(variable1,configuration->config[variable2],configuration->config[variable2])
    - configuration->get_conflicts(variable2,configuration->config[variable2],configuration->config[variable2])
    + configuration->get_conflicts(variable2,configuration->config[variable1],configuration->config[variable1])
    - configuration->get_conflicts(variable1,configuration->config[variable1],configuration->config[variable1]);
  if ((variable2-variable1) == (configuration->config[variable1] - configuration->config[variable2])
      ||   (variable1-variable2) == (configuration->config[variable1] - configuration->config[variable2]))
    valeur +=2;
  return valeur;
}


void SwNqueen::move_execution(Configuration* configuration,Move* move)
{OpProblem::move_execution(configuration,move);
 int aval = configuration->config[((SwapMove*) move)->variable1];
configuration->config[((SwapMove*) move)->variable1]=configuration->config[((SwapMove*) move)->variable2];
 configuration->config[((SwapMove*) move)->variable2]=aval;
}


void SwNqueen::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)

{int variable1 = ((SwapMove*)move)-> variable1;
  int variable2 = ((SwapMove*)move)-> variable2;
CSPMove move1;
move1.variable = variable1;
int ovalue1 =  configuration->config[variable1];
move1.value = configuration->config[variable2];
 update_conflicts1(configuration,& move1);
 Nqueen::move_execution(configuration, & move1);  // changement provisoire de la variable 1
move1.variable = variable2;
move1.value = ovalue1;

 update_conflicts1(configuration,& move1);
move1.variable = variable1;
 Nqueen::move_execution(configuration, & move1); // remise ancienne valeur variable 1
}


int SwNqueen::config_evaluation (Configuration* configuration)
{int valeur=0;
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   for(int j=i+1; j<nbvar;j++)
       {if (
	    (configuration->config[i]-configuration->config[j]== j-i)||
            (configuration->config[j]-configuration->config[i]== j-i))
	 {valeur++;}
       if (configuration->config[i] + j- i < nbvar)
	  configuration->incr_conflicts(j,configuration->config[i] + j -i,configuration->config[i] + j -i, 1);
       if (configuration->config[i] -( j- i) >=0)
	  configuration->incr_conflicts(j,configuration->config[i] - ( j -i),configuration->config[i] - ( j -i),1);
       if (configuration->config[j] + j- i < nbvar)
	 configuration->incr_conflicts(i,configuration->config[j] + j -i,configuration->config[j] + j -i,1);
       if (configuration->config[j] - (j- i) >= 0)
	 configuration->incr_conflicts(i,configuration->config[j] - (j -i),configuration->config[j] - (j -i),1);
       }

 return valeur;

}

void SwNqueen::update_conflicts1(Configuration* configuration,Move* move)

{int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  for (int i=0; i< var_changee; i++)

   {
     if (configuration->config[var_changee]-(var_changee-i) >= 0)
       configuration->incr_conflicts(i,configuration->config[var_changee]-(var_changee -i),configuration->config[var_changee]-(var_changee -i),-1);
     if (val_changee-(var_changee-i) >= 0)
       configuration->incr_conflicts(i,val_changee-(var_changee -i),val_changee-(var_changee -i),1);
     if (configuration->config[var_changee]+(var_changee-i) < nbvar)
       configuration->incr_conflicts(i,configuration->config[var_changee]+(var_changee -i),configuration->config[var_changee]+(var_changee -i),-1);
     if (val_changee+(var_changee-i) < nbvar)
       configuration->incr_conflicts(i,val_changee+(var_changee -i),val_changee+(var_changee -i),1);
   }
for (int i=var_changee+1; i< nbvar; i++)
{
   if (configuration->config[var_changee]-(i-var_changee) >= 0)
       configuration->incr_conflicts(i,configuration->config[var_changee]-(i-var_changee),configuration->config[var_changee]-(i-var_changee),-1);
     if (val_changee-(i-var_changee) >= 0)
       configuration->incr_conflicts(i,val_changee-(i-var_changee),val_changee-(i-var_changee),1);
     if (configuration->config[var_changee]+(i-var_changee) < nbvar)
       configuration->incr_conflicts(i,configuration->config[var_changee]+(i-var_changee),configuration->config[var_changee]+(i-var_changee),-1);
     if (val_changee+(i-var_changee) < nbvar)
       configuration->incr_conflicts(i,val_changee+(i-var_changee),val_changee+(i-var_changee),1);
   }
}

/* on ne construit pas explicitement l'ensemble des variables en conflit */
void SwNqueen::adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors)
    {var_conflict_size = compute_var_conflict_size(configuration);
    if 	(maxneighbors > var_conflict_size * (domainsize -1))
      maxneighbors = var_conflict_size * (domainsize -1);
    if 	(minneighbors > var_conflict_size * (domainsize -1))
      minneighbors = var_conflict_size * (domainsize-1);
    }

/* on ne construit pas explicitement l'ensemble des variables en conflit */
void SwNqueen::compute_var_conflict(Configuration* configuration)
{;}

int SwNqueen::random_conflict_variable(Configuration* configuration)
{int k = (int) (drand48() * var_conflict_size);
 int k1=-1;
 for (int i=0;i<nbvar;i++)
  {if (configuration->get_conflicts(i,configuration->config[i],configuration->config[i])!=0)
    k1++;
  if (k1==k) return i;}
 return k;

}


int SwNqueen::compute_var_conflict_size(Configuration* configuration)
{  int k=0;
for (int i=0;i<nbvar;i++)
{if (configuration->get_conflicts(i,configuration->config[i],configuration->config[i])!=0)
  k++;
  }
return k;
}


/* pour les mouvements en swap sans tabconflicts - variables en conflit n'est pas implanté*/


SwNiNqueen* swniqueen_problem_creation (int nbvar)
{
int* dom = new int[nbvar];

  vector<int>* tabdom = new vector<int> [nbvar] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar];
SwNiNqueen* problem = probleme_swnireines(nbvar);
problem->set_domains_connections(dom,tabdom,connex);
problem->init_domain_tabdomain();

 return problem;
}



SwNiNqueen::SwNiNqueen(int numvar) : SwNqueen(numvar) {};

Configuration* SwNiNqueen::create_configuration()
{return (new Configuration(nbvar));}

SwNiNqueen* probleme_swnireines(int nbvar)
{SwNiNqueen* p1 = new SwNiNqueen(nbvar);
return p1;
}

/*

int SwNiNqueen::move_evaluation (Configuration& configuration,int valeur,Move* move)
{
  int variable1 = ((SwapMove*)move)-> variable1;
  int variable2 = ((SwapMove*)move)-> variable2;

  for (int i=0; i< nbvar ; i++)
    {if (i != variable1 && i != variable2)
      {if
	  (abs (i - variable1) == abs (configuration.config[i] - configuration.config[variable1]))
	valeur--;
      if
	  (abs (i - variable2) == abs (configuration.config[i] - configuration.config[variable2]))
	valeur--;
      if
	  (abs (i - variable1) == abs (configuration.config[i] - configuration.config[variable2]))
	valeur++;
      if
	  (abs (i - variable2) == abs (configuration.config[i] - configuration.config[variable1]))
	valeur++;
      }
    }
  return valeur;
}
*/

/* meme fonction sans les valeurs absolues : 2 fois plus rapide */

int SwNiNqueen::move_evaluation (Configuration* configuration,Move* move)
{
  int variable1 = ((SwapMove*)move)-> variable1;
  int variable2 = ((SwapMove*)move)-> variable2;
  int valeur = configuration->valuation;
  for (int i=0; i< nbvar ; i++)
    {if (i != variable1 && i != variable2)
      {if
	  (i - variable1 == configuration->config[i] - configuration->config[variable1]
			    ||
	  i - variable1 ==  configuration->config[variable1] -configuration->config[i] )
	valeur--;
      if
	  (i - variable2 == configuration->config[i] - configuration->config[variable2]
           ||
	   i - variable2 == configuration->config[variable2] -configuration->config[i] )
	valeur--;
      if
	  (i - variable1 == configuration->config[i] - configuration->config[variable2]
||
	   i - variable1 ==  configuration->config[variable2] -configuration->config[i])
	valeur++;
      if
	  (i - variable2 == configuration->config[i] - configuration->config[variable1]
	   ||
	   i - variable2 == configuration->config[variable1] - configuration->config[i]         )
	valeur++;
      }
    }
  return valeur;
}





int SwNiNqueen::config_evaluation (Configuration* configuration)
{int valeur=0;
 for(int i=0; i<nbvar; i++)
   for(int j=i+1; j<nbvar;j++)
       {if (
	    (configuration->config[i]-configuration->config[j]== j-i)||
            (configuration->config[j]-configuration->config[i]== j-i))
	 {valeur++;
	 }
       }
 return valeur;

}



void SwNiNqueen::adjust_parameters(Configuration* configuration, int& maxneighbors, int& minneighbors) {};


