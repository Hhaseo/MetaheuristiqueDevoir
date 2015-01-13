/* le celar en tout-incremental : implanté pour celar 6,7et 8*/




#include <list>
#include <vector>
#include <string>
#include <set>
using namespace std;
#include <fstream>
#include "incop.h"
#include "csproblem.h"
#include "celar.h"



extern ofstream* ofile;  // le fichier de sortie


Configuration* CelarCSProblem::create_configuration ()
{return  (new FullincrCSPConfiguration(nbvar,domainsize));
}



/* redéfinition des variables en conflit surtout utile pour celar7 :
 le fait qu'une variable soit en conflit dépend de la valeur de la configuration courante */
// optimise pour fullincr 


void CelarCSProblem::compute_var_conflict(Configuration* configuration)
{configuration->var_conflict.clear();
 for (int i=0;i<nbvar;i++)
   { int nbconf = ((FullincrCSPConfiguration*)configuration)->tabconflicts[i][value2index(configuration->config[i],i)];
   if ( (nbconf > 1000000)  ||
      ( (nbconf> 10000) && (configuration->valuation < 20000000)) ||
      ( (nbconf > 100)  && (configuration->valuation < 800000)) ||
      ((nbconf > 0)  && (configuration->valuation < 400000)))
    configuration->var_conflict.push_back(i);
   }
 //*ofile << " nb var conflits " << configuration->var_conflict.size() << endl;
}

 
// critere additif des celar 6,7,8  -  implantation pour  tout  incr


int CelarCSProblem::config_evaluation(Configuration* configuration)
{int valeur=0;
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   for(int j=i+1; j<nbvar; j++)
     if (constraints[i][j])
       {if (abs (configuration->config[i] - configuration->config[j])
	    <= distance[constraints[i][j]])
	 {valeur+= valconst[constraints[i][j]];
	 }
       for (int k =0; k<(int)(tabdomains[domains[i]].size());k++)
	 if (abs (configuration->config[j] - tabdomains[domains[i]][k])
	     <= distance[constraints[i][j]])
	   configuration->incr_conflicts(i,tabdomains[domains[i]][k] , k, valconst[constraints[i][j]]);
       for (int k =0; k<(int)(tabdomains[domains[j]].size());k++)
	 if (abs (configuration->config[i] - tabdomains[domains[j]][k])
	     <= distance[constraints[i][j]])
	   configuration->incr_conflicts(j, tabdomains[domains[j]][k] ,k, valconst[constraints[i][j]]);
       }
 return valeur;
}


      





/* optimisation pour le cas où la Configuration est fullincr */


int CelarCSProblem::move_evaluation1(Configuration* configuration,int valeur,int var_changee,int val_changee)
{
  return(valeur+((FullincrCSPConfiguration*)configuration)->tabconflicts[var_changee][value2index(val_changee,var_changee)]
									 
               -((FullincrCSPConfiguration*)configuration)->tabconflicts[var_changee][value2index(configuration->config[var_changee],var_changee)]);
}


