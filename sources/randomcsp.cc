#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
using namespace std;
#include <iostream>
#include <fstream>
#include "timer.h"
#include "incop.h"
#include "csproblem.h"
#include "incoputil.h"
#include "urbcsp2.h"
#include "extcsp.h"
#include "autotuning2.h"

#include <math.h>
#include <unistd.h>

extern ofstream* ofile;  // le fichier de sortie

extern Stat_GWW * Statistiques; 

void arguments_csp_alea (char** argv, int& narg, int& nbvar, int& s, int & nbconst, int& p2, 
int & graine, int & npb)
{      nbvar = argument2ul(argv[narg+1], " nombre de variables");
      s = argument2ul(argv[narg+2], " taille des domaines ");

      nbconst = argument2bul(argv[narg+3]," nombre de contraintes ", 0, (nbvar*(nbvar-1))/2);
      p2 = argument2bul (argv[narg+4], " nombre de couples interdits par contrainte ", 0, s*s);

      graine = argument2ul (argv[narg+5], " graine du générateur aléatoire ");
      npb= argument2ul(argv[narg+6], " nombre de problèmes ");
      *ofile << " Csp aléatoire " << " nb variables " << nbvar << " taille domaines " << s << " nb contraintes " 
	   << nbconst << " graine générateur " << graine << " nb problèmes " << npb << endl;
      narg=narg+6;}


ExtensionBinaryCSProblem* probleme_randomcsp(int nbvar, int nbconst, int s, int ** constraint1, int*** constraint2, int t, int graine,int p2)
{ExtensionBinaryCSProblem*  problem =new  ExtensionBinaryCSProblem (nbvar,nbconst,s);
 
	  /*appel au generateur de CSP aleatoires  qui remplit les tableaux
	    constraint1 et constraint2*/

 evolution_urbcsp2 (nbvar, s, nbconst, p2, t+graine+1, 1, constraint1,constraint2);
 problem->constraints=constraint1;
 problem->constraint2=constraint2;
 return problem;
}


ExtensionBinaryCSProblem* random_problem_creation (int nbvar, int nbconst, int s, int ** constraint1, int*** constraint2, int t, int graine,int p2, vector<int>* connexions)
{
  extensionbinary_inittuples(nbconst,s,constraint2,1);  //   tabu = 1 :les couples donnés sont les couples interdits
  ExtensionBinaryCSProblem* problem = probleme_randomcsp(nbvar,nbconst,s,constraint1,constraint2,t,graine,p2);
  extensionbinary_setconnexions(nbvar,connexions,constraint1);
  return problem;
}


int randomcsp(int argc,char** argv, int tuningmode)
{
  // les divers arguments lus dans la ligne de commande
  int nbvar,nbconst;
  int s,taille,p2,graine,nbessais;
  int graine1;
  int narg = 1;  // compteur des arguments
  int npb;
  // le nom du fichier de sortie : pour les tests : version logiciel + concaténation des arguments
  char filename [1000];
  if ((string)argv[1] == "arg")
    ofile_name(filename, argc, argv);
  else sprintf(filename,"%s",argv[1]);

  
  ofstream ofile1 (filename);
  ofile = & ofile1;


  // lecture des arguments du problème
  arguments_csp_alea(argv, narg, nbvar, s,nbconst, p2, graine, npb);

  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (npb, nbessais);
  

  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la recuperation du signal 10
  sigaction();
 // argument de temps maximum 
  double maxtime;
  if (tuningmode)
    arguments_tempscpu (argv,narg,maxtime);

  // argument arret
  int stop=0;
  if (tuningmode)
    arguments_arret (argv,narg,stop);
  Statistiques->stop_trouve=stop;  

  // Declaration des variables contenant les structures de données des problemes
  int **constraint1; // utilise dans tout csp binaire
  int*** constraint2;
  int domaines[nbvar];  // le numero du type de domaine de la variable
  
  vector<int> tabdomaines[nbvar] ; // les differents types de domaines 
  // Initialisation des structures de données des problèmes
  vector<int> connexions[nbvar];

  constraint1 =  csp_constraintdatastructure(nbvar);
  constraint2 = extensionbinary_tupledatastructure (nbconst,s);
  for (int t=0;t<npb  ;t++)
    {  Statistiques->init_pb(t);
    Statistiques->total_problem_time[t]=0;
    /* Initialisation des tableaux */
    for(int i=0;i<nbvar;i++) 
      {for(int j=0;j<nbvar;j++)
	constraint1[i][j]=0;}
    *ofile<< "probleme " << t+1 << endl;
    
    ExtensionBinaryCSProblem*  problem = random_problem_creation (nbvar,nbconst,s,constraint1,constraint2,t,graine,p2, connexions);

	  
    // mise en place des domaines et connexions

    problem->set_domains_connections(domaines,tabdomaines,connexions);
    problem->init_domain_tabdomain();
    // creation de la population et initialisation 
    // La population : tableau de configurations
    Configuration* population[taille];
    problem->init_population(population,taille);
    
    problem->allocate_moves();

    // boucle sur les essais 
    if (tuningmode) 
      {    int premieremarche=100000;
      
      autosolving((LSAlgorithm*)algo,population,problem,t,graine1,nbessais,maxtime,premieremarche);
      }
    else 
      {
  // boucle sur les essais 

	for(int nessai = 0;nessai< nbessais ; nessai++)
	  executer_essai (problem,algo,population,taille,graine1,nessai);
	
	// ecriture statistiques 
	Statistiques->current_try++; 
	ecriture_stat_probleme();
      }
    delete problem;
    Statistiques->total_problem_time[t]=Statistiques->total_execution_time;
    Statistiques->total_execution_time =0;  // utilise pour arreter chaque pb
    }
  if (npb > 1)
    {ecriture_statistiques_global ();
    *ofile << " temps par probleme " ;
    for (int i=0;i<npb;i++)
      *ofile << Statistiques->total_problem_time[i] << " | "; 
    float temps_total =0;
    for (int i=0; i< npb; i++)
      temps_total+=Statistiques->total_problem_time[i];
    *ofile << endl;
    *ofile << " temps total " <<  temps_total << endl;
    *ofile << " temps moyen " << temps_total / npb << endl;
    
    }

  cout << "Fin résolution " <<  endl;
  return 0;
  
  
}










