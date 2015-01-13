
#include <list>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
using namespace std;
#include <fstream>
#include "incop.h"
#include "csproblem.h"
#include "extcsp.h"

extern ofstream* ofile;  // le fichier de sortie


// CSP Binaires en extension 

ExtensionBinaryCSProblem::ExtensionBinaryCSProblem (int nvar, int nconst, int domain) :
BinaryCSProblem (nvar,nconst){domainsize=domain;}

Configuration* ExtensionBinaryCSProblem::create_configuration()
{return (new FullincrCSPConfiguration (nbvar, domainsize));}

void extensionbinary_setconnexions(int nbvar, vector<int>* connexions, int** constraint1)
{  for(int i=0; i<nbvar;i++)
  {connexions[i].clear();
  }
 for (int i=0;i<nbvar;i++)
   for(int j=i+1;j<nbvar;j++)
     if (constraint1[i][j] != 0)
       {connexions[i].push_back(j);
       connexions[j].push_back(i);
       }
}

int*** extensionbinary_tupledatastructure (int nbconst, int s)
{ int***  constraint2= new int**[nbconst];
 for(int i=0;i<nbconst;i++)
   {
     constraint2[i]=new int*[s];
     for(int j=0;j<s;j++){
       constraint2[i][j]=new int[s];}
   }
 return constraint2;
}


void extensionbinary_inittuples(int nbconst, int s, int*** constraint2, int tabu)
{ for(int i=0;i<nbconst;i++)
	    {
	      for(int j=0;j<s;j++){
		for(int k=0;k<s;k++)
		  {if (tabu) constraint2[i][j][k]=0;
		  else constraint2[i][j][k]=1;}
	      }
	    }
}

/** Evaluation d'une configuration (nombre de contraintes violées) et remplissage des structures de données des conflits*/


int ExtensionBinaryCSProblem::config_evaluation (Configuration* configuration)
{int valeur=0;
 int conflit=0; 
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   for(int j=i+1; j<nbvar;j++)
     {
     if (constraints[i][j])
       {
	 conflit= constraint2[constraints[i][j]-1] [configuration->config[i]] [configuration->config[j]];
	 //	 if (conflit) *ofile << " i " << i << " j " << j << endl;
	 if (conflit) valeur++;
	 for (int k=0;k<domainsize;k++)
	   { configuration->incr_conflicts (j,k,k, constraint2[constraints[i][j]-1] [configuration->config[i]] [k]);
	   configuration->incr_conflicts(i,k,k, constraint2[constraints[i][j]-1] [k] [configuration->config[j]]);
	   }
       }
     }
 return valeur;
}

/** mise à jour de la structure de données tabconflicts (cas fullincr) */
void ExtensionBinaryCSProblem::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)

{  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
 for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
  {
  if (*vi < var_changee)
    for (int k=0;k<domainsize;k++)
      configuration->incr_conflicts(*vi,k,k,
	constraint2[constraints[*vi][var_changee]-1][k][val_changee]
	- constraint2[constraints[*vi][var_changee]-1][k][configuration->config[var_changee]]);
  else 
    for (int k=0;k<domainsize;k++)
      configuration->incr_conflicts(*vi,k,k,
	constraint2[constraints[var_changee][*vi]-1][val_changee][k]
	- constraint2[constraints[var_changee][*vi]-1][configuration->config[var_changee]][k]);
  }
}

/** mise à jour de la structure de données tabconflicts (cas incr) */
void ExtensionBinaryCSProblem::incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move)

{  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
 for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
  {if (*vi < var_changee)
      configuration->incr_conflicts(*vi,configuration->config[*vi],0,
	constraint2[constraints[*vi][var_changee]-1][configuration->config[*vi]][val_changee]
	- constraint2[constraints[*vi][var_changee]-1][configuration->config[*vi]][configuration->config[var_changee]]);
  else 
    configuration->incr_conflicts(*vi,configuration->config[*vi],0,
	  constraint2[constraints[var_changee][*vi]-1][val_changee][configuration->config[*vi]]
	- constraint2[constraints[var_changee][*vi]-1][configuration->config[var_changee]][configuration->config[*vi]]);
  }
  configuration->set_variableconflicts(var_changee, configuration->get_conflicts_problem(this,var_changee,val_changee));
}

/** calcule les conflits de (var,val) avec la configuration courante */
int ExtensionBinaryCSProblem::compute_conflict(Configuration* configuration, int var , int val)
{int conflict=0;
 for (vector<int>::iterator vi= connections[var].begin();vi!= connections[var].end(); vi++)
   {if (*vi < var) 
     conflict+=constraint2[constraints[*vi][var]-1][configuration->config[*vi]][val];
   else
     conflict+=constraint2[constraints[var][*vi]-1][val][configuration->config[*vi]];
   }
 return conflict;
}


void ExtensionBinaryCSProblem::solution_write(){;}
