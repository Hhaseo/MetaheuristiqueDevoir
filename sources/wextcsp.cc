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
#include  "wextcsp.h"

extern ofstream* ofile;  // le fichier de sortie



void wextensionbinary_setconnexions(int nbvar, vector<int>* connexions, vector<int>** constraint1)
{  for(int i=0; i<nbvar;i++)
  {connexions[i].clear();
  }
 for (int i=0;i<nbvar;i++)
   for(int j=i+1;j<nbvar;j++)
     if (constraint1[i][j].size() != 0)
       {connexions[i].push_back(j);
       connexions[j].push_back(i);
       }
}


// seules différences avec ExtensionBinaryCSProblem : introduction d'un poids pour chaque contrainte 
// et vecteur de contraintes par paire de variable (i,j)  , i<j

WeightExtensionBinaryCSProblem::WeightExtensionBinaryCSProblem (int nvar, int nconst, int domsize) 
  : ExtensionBinaryCSProblem(nvar,nconst,domsize) {;}


Configuration* WeightExtensionBinaryCSProblem::create_configuration ()
{return (new IncrCSPConfiguration (nbvar,domainsize));}


/** Evaluation d'une configuration (somme pondérée de contraintes violées) et remplissage des structures de données des conflits*/


int WeightExtensionBinaryCSProblem::config_evaluation (Configuration* configuration)
{int valeur=0;
 int conflit=0; 
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   for(int j=i+1; j<nbvar;j++)
     {
     for (int cc= 0; cc< (int) lconstraints[i][j].size(); cc++)
       { int valuei = configuration->config[i];
         int valuej = configuration->config[j];
	 conflit= constraint2[lconstraints[i][j][cc]] [valuei] [valuej];
	 if (conflit) valeur+= cstweights[lconstraints[i][j][cc]];
	 for (int k=0;k< variable_domainsize (i);k++)
	   { 
             configuration->incr_conflicts(i,k,k, cstweights[lconstraints[i][j][cc]] * constraint2[lconstraints[i][j][cc]] [k] [valuej]);
	   }
	 for (int k=0;k<variable_domainsize(j);k++)
	   {configuration->incr_conflicts (j,k,k, cstweights[lconstraints[i][j][cc]] * constraint2[lconstraints[i][j][cc]] [valuei] [k]);
	   }
       }
     }
 return valeur;
}

/** mise à jour de la structure de données tabconflicts (cas fullincr) */
void WeightExtensionBinaryCSProblem::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)

{  int var_changee = ((CSPMove*)move)-> variable;
 int val_changee = ((CSPMove*)move)-> value;
 for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
   {if (*vi < var_changee)
     for (int cc=0;cc < (int) lconstraints[*vi][var_changee].size(); cc++)
       for (int k=0;k<variable_domainsize(*vi);k++)

	 configuration->incr_conflicts(*vi,k,k, 
				       cstweights[lconstraints[*vi][var_changee][cc]] * constraint2[lconstraints[*vi][var_changee][cc]][k][val_changee]
				       - cstweights[lconstraints[*vi][var_changee][cc]] * constraint2[lconstraints[*vi][var_changee][cc]][k][configuration->config[var_changee]]);
   else 
     for (int cc=0;cc < (int) lconstraints[var_changee][*vi].size(); cc++)
       for (int k=0;k<variable_domainsize (*vi);k++)

	 configuration->incr_conflicts(*vi,k,k,
				       cstweights[lconstraints[var_changee][*vi][cc]] *  constraint2[lconstraints[var_changee][*vi][cc]][val_changee][k]
				       - cstweights[lconstraints[var_changee][*vi][cc]] * constraint2[lconstraints[var_changee][*vi][cc]][configuration->config[var_changee]][k]);
   }
}

/** mise à jour de la structure de données tabconflicts (cas incr) */
void WeightExtensionBinaryCSProblem::incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move)

{  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
    {if (*vi < var_changee)
      for (int cc =0; cc< (int) lconstraints[*vi][var_changee].size(); cc++)
	configuration->incr_conflicts(*vi,configuration->config[*vi],0, 
				      cstweights[lconstraints[*vi][var_changee][cc]] * constraint2[lconstraints[*vi][var_changee][cc]][configuration->config[*vi]][val_changee]
				      - cstweights[lconstraints[*vi][var_changee][cc]] * constraint2[lconstraints[*vi][var_changee][cc]][configuration->config[*vi]][configuration->config[var_changee]]);
    else 
      for (int cc =0; cc< (int) lconstraints[var_changee][*vi].size(); cc++)
	configuration->incr_conflicts(*vi,configuration->config[*vi],0,
				      cstweights[lconstraints[var_changee][*vi][cc]] * constraint2[lconstraints[var_changee][*vi][cc]][val_changee][configuration->config[*vi]]
				      - cstweights[lconstraints[var_changee][*vi][cc]] * constraint2[lconstraints[var_changee][*vi][cc]][configuration->config[var_changee]][configuration->config[*vi]]);
    }
  configuration->set_variableconflicts(var_changee, configuration->get_conflicts_problem(this,var_changee,val_changee));
}


/** calcule les conflits de (var,val) avec la configuration courante */
int WeightExtensionBinaryCSProblem::compute_conflict(Configuration* configuration, int var , int val)
{int conflict=0;
 for (vector<int>::iterator vi= connections[var].begin();vi!= connections[var].end(); vi++)
   {if (*vi < var) 
     for (int cc =0; cc< (int) lconstraints[*vi][var].size(); cc++)
       conflict+=cstweights[lconstraints[*vi][var][cc]] * constraint2[lconstraints[*vi][var][cc]][configuration->config[*vi]][val];
   else
     for (int cc =0; cc< (int) lconstraints[var][*vi].size(); cc++)
       conflict+=cstweights[lconstraints[var][*vi][cc]] * constraint2[lconstraints[var][*vi][cc]][val][configuration->config[*vi]];
   }
 return conflict;
}


