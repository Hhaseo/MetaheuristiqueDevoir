/* L'implantation semi-incrementale (IncrCSPConfiguration), avec les méthodes spécifiques pour cette implantation */



#include <list>
#include <vector>
#include <string>
#include <set>
using namespace std;
#include <fstream>
#include "incop.h"
#include "csproblem.h"
#include "celar.h"



Configuration* CelarCSProblem::create_configuration()
{ return (new IncrCSPConfiguration(nbvar,domainsize));
}





/* redéfinition des variables en conflit surtout utile pour celar7 :
 le fait qu'une variable soit en conflit dépend de la valeur de la configuration courante */
/* optimisation incr   */

void CelarCSProblem::compute_var_conflict(Configuration* configuration)
{configuration->var_conflict.clear();
 for (int i=0;i<nbvar;i++)
   {if (!varcost || refvalue[i] ==0 || refcost[i]!=0)  // variable modifiable
     {   int nbconf= ((IncrCSPConfiguration*)configuration)->tabconflicts[i];
   if ( (nbconf > 1000000)  ||
      ( (nbconf> 10000) && (configuration->valuation < 20000000)) ||
      ( (nbconf > 100)  && (configuration->valuation < 800000)) ||
      ((nbconf > 0)  && (configuration->valuation < 400000)))
    configuration->var_conflict.push_back(i);
     }
   }
//*ofile << " nb var conflits " << configuration->var_conflict.size() << endl;
}





// optimisation incr

int CelarCSProblem::config_evaluation(Configuration* configuration)
{int valeur=0;
configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   // pour celar 9 et celar 10
   {if (varcost && refvalue[i]!=0)
     if (configuration->config[i] != refvalue[i])
       {valeur+= refcost[i];
       configuration->incr_conflicts(i,configuration->config[i],0,refcost[i]);}
   for(int j=i+1; j<nbvar; j++)
     if (constraints[i][j])
       {if (abs (configuration->config[i] - configuration->config[j])
	    <= distance[constraints[i][j]])
	 {valeur+= valconst[constraints[i][j]];
         configuration->incr_conflicts(i,configuration->config[i],0,valconst[constraints[i][j]]);
         configuration->incr_conflicts(j,configuration->config[j],0, valconst[constraints[i][j]]);
	 }
       }				      
   }
 return valeur;
}





int CelarCSProblem::move_evaluation1(Configuration* configuration,int valeur,int var_changee,int val_changee)

{
  return(valeur+compute_conflict(configuration,var_changee,val_changee)
               -((IncrCSPConfiguration*)configuration)->tabconflicts[var_changee]);
}




