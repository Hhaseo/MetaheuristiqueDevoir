#include <cerrno>
#include <stdio.h>

#include <list>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

using namespace std;
#include <fstream>
#include <iostream>
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "color.h"
#include "autotuning2.h"


extern ofstream* ofile;  // le fichier de sortie

extern Stat_GWW * Statistiques; // trombe_ajout: l'objet pour les stats en var globale

/* Tout ce qui concerne le coloriage de graphes (color + impasse)*/

void arguments_coloriage(char** argv, int& narg, int & nbcol)
{ 
  nbcol = argument2ul(argv[narg+1],"nombre de couleurs ");
  *ofile << " coloriage  " << argv[narg+1] << "  nb couleurs " << nbcol << endl;
  narg++;
}


void lire_debut_fichier_coloriage(ifstream & file, int& nbvar, int& nbconst)
{
  string tt,tt1;
  char cc[256];
  /* lecture des premieres lignes du fichier : nombre de variables
     et de contraintes */
  file >> tt1;
  while (tt1=="c")
     { file.getline(cc,255); // une ligne de commentaires
     *ofile << "CC " << cc << endl;
     file >> tt1; }
  file >> tt1; // edge
  file >> nbvar; file >> nbconst; 
  *ofile << "Nb noeuds " << nbvar << " Nb aretes " << nbconst << endl;
}

// remplissage des structures de donnees des contraintes : constraint1 et connexions
void lire_fichier_coloriage (ifstream& file, vector<int>* connexions,int ** constraint1)
{       string tt1;
        int i1,i2;
	file >> tt1;
      while(!file.eof()) 	
	{         
	  file >> i1 ; file >> i2; 
	   
          if(find(connexions[i1-1].begin(), 
		  connexions[i1-1].end(), i2-1) == connexions[i1-1].end())
	    {
             connexions[i1-1].push_back(i2-1);
	     connexions[i2-1].push_back(i1-1);
	    }
	  if (i1 < i2)  
	    constraint1[i1-1][i2-1]=1;
	  else if (i1 > i2) 
	    constraint1[i2-1][i1-1]=1;
	  //	  cout  << " Arete " <<i1 << " " << i2 << endl;

	  file >> tt1;	
	}
}



ColorCSProblem* color_problem_creation (int s,ifstream & file)
{

  // Declaration des variables contenant les structures de données des problemes
  int **constraint1; // utilise dans tout csp binaire
  int nbvar; int nbconst;
  lire_debut_fichier_coloriage(file,nbvar,nbconst); // pour dimensionner les structures
  
  constraint1 =  csp_constraintdatastructure(nbvar);

  int* dom = new int[nbvar];
  
  vector<int>* tabdom = new vector<int> [nbvar] ; // les differents types de domaines 
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar];

  lire_fichier_coloriage(file ,connex,constraint1);
  ColorCSProblem * problem = probleme_coloriage(nbvar,nbconst,s,constraint1);
  // mise en place des domaines et connexions

  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;
}

/* un seul type de domaine */
void ColorCSProblem::init_domains(int nbvar, int s)
{for (int i=0;i<nbvar;i++)
  domains[i]=0;
}

// optimisation pour mouvement tout incremental 

int ColorCSProblem::move_evaluation 
                    (Configuration* configuration,Move* move)
{ int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  return (configuration->valuation+ ((FullincrCSPConfiguration*)configuration)->tabconflicts[var_changee][val_changee]
	  -((FullincrCSPConfiguration*)configuration)->tabconflicts[var_changee][configuration->config[var_changee]]);
}


/* variante avec suppression des symétries : plutot nuisible : abandonné
void ColorCSProblem::init_tabdomaines(int s)
{   CSProblem::init_tabdomains(s);
  for (int i=0;i<s;i++) 
  {
    tabdomains[i].clear();
    for (int j=0;j<i+1;j++)
      tabdomains[i].push_back(j);
  }
}
*/

int colorcsp (int argc, char** argv, int tuningmode) {

  // les divers arguments lus dans la ligne de commande

  int nbcol,taille,nbessais;
  int graine1;

  int narg = 2;  // compteur des arguments 
  // le nom du fichier de sortie en 1er argument : pour les tests  arg ->  version logiciel + concaténation des arguments
  char filename [1000];
  if ((string)argv[1] == "arg")
    ofile_name(filename, argc, argv);
  else sprintf(filename,"%s",argv[1]);
      
  ofstream ofile1 (filename);
  ofile = & ofile1;

  ifstream file (argv[2]); // le fichier de données 

  // lecture des arguments du problème
  arguments_coloriage(argv,narg,nbcol);

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
  if (tuningmode) arguments_tempscpu (argv,narg,maxtime);

  // creation du probleme (lecture des données, création des structures de données et du problème)
  ColorCSProblem* problem = color_problem_creation (nbcol,file);

  // creation de la population et initialisation 
  // La population : tableau de configurations
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->allocate_moves();

  // initialisation des statistiques
  Statistiques->init_pb(0);
 
  if (tuningmode)
    // lancement reglage + essais
    autosolving((LSAlgorithm*)algo,population,problem,0,graine1,nbessais,maxtime,1000000);
  else
    // boucle sur les essais 
    {for(int nessai = 0;nessai< nbessais ; nessai++)
      executer_essai (problem,algo,population,taille,graine1,nessai);

    // ecriture statistiques 
    Statistiques->current_try++; 
    ecriture_stat_probleme();}

  delete problem;
  cout << "Fin résolution " << Statistiques->total_execution_time << endl;
  return 0;
  
  
}






ColorCSProblem::ColorCSProblem (int nvar, int nconst, int nbcolor) :
BinaryCSProblem (nvar,nconst){domainsize=nbcolor;}


ColorCSProblem* probleme_coloriage(int nbvar,int nbconst,int s,int** constraint1)
{ColorCSProblem* p1 = new ColorCSProblem(nbvar,nbconst,s);
p1->constraints=constraint1; 
return p1;
}



// analyse des conflits d'une configuration : recherche des composantes connexes dans l'ensemble
// des variables en conflit



void ColorCSProblem::analyse_conflicts(Configuration* configuration)
{ 
 list<int> noeuds_conflit;
 for(int i=0; i< nbvar; i++)
   for(int j=i+1; j<nbvar;j++)
     if (constraints[i][j])
       if (configuration->config[i]==configuration->config[j]) 
	 {if (find(noeuds_conflit.begin(),noeuds_conflit.end(),i)==noeuds_conflit.end())
	   noeuds_conflit.push_back(i);
	 if  (find(noeuds_conflit.begin(),noeuds_conflit.end(),j)==noeuds_conflit.end())
	   noeuds_conflit.push_back(j);
	 }
 *ofile << "nb variables conflit " << noeuds_conflit.size()<< endl ;
 int composante_connexe=0;
 int nb_sup_couleurs=0;
 while(noeuds_conflit.size()!=0)
 {  int i=noeuds_conflit.front();
    noeuds_conflit.pop_front();
    composante_connexe++;
    list<int> connexes;
    list<int> connectes;
    *ofile << i << " " ;
    connectes.push_back(i);
    list<int>::iterator li;
    for (li=noeuds_conflit.begin();li!=noeuds_conflit.end();li++)
      if ((i<*li && constraints[i][*li]) || 
	  (i>*li && constraints[*li][i]))
	{if (find (connectes.begin(),connectes.end(),*li) == connectes.end())
	  {*ofile << *li << " " ;
	  connexes.push_back(*li); connectes.push_back(*li);}
	}
    for (li=connectes.begin();li!=connectes.end();li++)
      noeuds_conflit.remove(*li);
    while(connexes.size()!=0)
      {int j= connexes.front();
	 connexes.pop_front();
	 for (li=noeuds_conflit.begin();li!=noeuds_conflit.end();li++)
	   if ((j<*li && constraints[j][*li]) || 
	       (j>*li && constraints[*li][j]))
	     if (find (connectes.begin(),connectes.end(),*li) == connectes.end())
	       {*ofile << *li << " " ;
	       connexes.push_back(*li); connectes.push_back(*li);}
      }
    for (li=connectes.begin();li!=connectes.end();li++)
      noeuds_conflit.remove(*li);
    int sup_couleurs=0;
    for (li=connectes.begin();li!=connectes.end();li++)
       {list<int>::iterator lj;
       for (lj=connectes.begin();lj!=connectes.end();lj++)
	 {if (*li < *lj &&
	      constraints[*li][*lj] && 
	      configuration->config[*li]==configuration->config[*lj])
	   sup_couleurs ++;
	 }
       }
    if (sup_couleurs > nb_sup_couleurs) 
      nb_sup_couleurs = sup_couleurs;
       	 

    *ofile << " taille composante connexe "  <<  connectes.size() << endl;
 }
 *ofile << " nombre couleurs supplementaires " << nb_sup_couleurs << endl;

}


void ColorCSProblem::best_config_analysis() {;}
/*
void ColorCSProblem::best_config_analysis() 
{
  Configuration* configu;
  configu = new FullincrCSPConfiguration(nbvar,domainsize);
  //  cout << " objet configuration cree " << endl;
  for (int i=0; i<nbvar; i++)
    {
    configu->config[i]= best_config->config[i];
    }
  //  cout << " valeur meilleure config " << config_evaluation(configu) << endl;
  analyse_conflicts(configu);
}
*/
/*
int ColorCSProblem::random_conflict_variable(Configuration* configuration)
{// *ofile << "VC " << configuration->var_conflict.size() << endl;
  // return configuration->var_conflict[(int) (drand48() * configuration->var_conflict.size())];
  //  return configuration->var_conflict[rand() % configuration->var_conflict.size()];
  configuration->trynumber++;
 return configuration->var_conflict[configuration->trynumber % configuration->var_conflict.size()];
}
*/
void ColorCSProblem::random_configuration(Configuration* configuration)
{
      for(int j=0;j<nbvar;j++)
	{int indice = (int) (drand48() * variable_domainsize(j));
      	configuration->config[j]= index2value(indice,j);
	}
}


/** nombre de contraintes de difference violées - on met en place les compteurs de conflits par variable */

int ColorCSProblem::config_evaluation (Configuration* configuration)
{int value=0;
 configuration->init_conflicts();
 for(int i=0; i<nbvar; i++)
   for (int j=i+1; j< nbvar; j++)
     if (constraints[i][j])
       {if (configuration->config[i]==configuration->config[j]) 
	 {value++; 
	 }
       configuration->incr_conflicts(i,configuration->config[j],configuration->config[j],1);
       configuration->incr_conflicts(j,configuration->config[i],configuration->config[i],1);
       }
 return value;
}




/* la premiere variable est fixee (suppression symetries) : plutot nuisible -> abandonné
int ColorCSProblem::random_variable(Configuration* configuration)
{return (1+ (int) (drand48() * (nbvar-1)));}
*/


/** appelé par le calcul non incremental et par le semi-incrémental pour les valeurs autres que la courante*/

int ColorCSProblem::compute_conflict(Configuration* configuration, int var , int val)
{int conflict=0;
 for (vector<int>::iterator vi= connections[var].begin();vi!= connections[var].end(); vi++)
   {if (val == configuration->config[*vi])
     conflict++;
   }
 // *ofile << " var " << var << " val " << val <<  " nbconflits " << conflict << endl;
return conflict;
}


// mise à jour des conflits - stockage fullincr
/*
void ColorCSProblem::fullincr_update_conflicts(Configuration* configuration,Move* move)
{int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
 for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
   {
     configuration->incr_conflicts(*vi,configuration->config[var_changee],configuration->config[var_changee],-1);
     configuration->incr_conflicts(*vi,val_changee,val_changee,1);
   }
}
*/

/** mise à jour des conflits dans l'implantation avec incrémentalité complète */
// même fonction optimisée 
// optimisation : utilisation directe  de la  structure de données

void ColorCSProblem::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)
{int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
   {
     configuration->tabconflicts[*vi][configuration->config[var_changee]]--;
     configuration->tabconflicts[*vi][val_changee]++;
   }
}


/** mise à jour des conflits dans l'implantation incr (seuls les nb de conflits des valeurs courantes sont stockés)*/
void ColorCSProblem::incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move)
{int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
 for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
   {
     configuration->incr_conflicts(*vi,configuration->config[var_changee],configuration->config[var_changee],-1);
     configuration->incr_conflicts(*vi,val_changee,val_changee,1);
   }
 // uniquement pour incr
 configuration->set_variableconflicts(var_changee, configuration->get_conflicts_problem(this,var_changee,val_changee));
}






void ColorCSProblem::best_config_write()
{CSProblem::best_config_write();}


Configuration* ColorCSProblem::create_configuration()
{return (new FullincrCSPConfiguration (nbvar,domainsize ));}



/*
void ColorCSProblem::compute_var_conflict(Configuration* configuration) 
{configuration->var_conflict.clear();
configuration->var_conflict_size=0;
 for (int i=0;i<nbvar;i++)
  {if (((FullincrCSPConfiguration*)configuration)->tabconflicts[i][configuration->config[i]])
    {configuration->var_conflict_size++;
     for (int k=0; k < ((FullincrCSPConfiguration*)configuration)->tabconflicts[i][configuration->config[i]]; k++)
       configuration->var_conflict.push_back(i);
    }
  }
}
*/

// méthode redéfinie pour raison d'efficacité avec Configuration FullIncr
void ColorCSProblem::compute_var_conflict(Configuration* configuration) 
{configuration->var_conflict.clear();
 for (int i=0;i<nbvar;i++)
   if (((FullincrCSPConfiguration*)configuration)->tabconflicts[i][configuration->config[i]])
     configuration->var_conflict.push_back(i);

}



/*
void ColorCSProblem::adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors)
  { int var_conflict_size = configuration->var_conflict_size;
    if 	(maxneighbors > var_conflict_size * (domainsize -1))
      maxneighbors = var_conflict_size * (domainsize -1);
    if 	(minneighbors > var_conflict_size * (domainsize -1))
      minneighbors = var_conflict_size * (domainsize -1);
  }
*/


// méthode redefinie pour raison d'efficacité avec Configuration Fullincr
int ColorCSProblem::min_conflict_value(int var, int val, Configuration* configuration)
{  // if (variable_domainsize(var)==1) return 0; // domaine a une valeur
   int minpromises=RAND_MAX; int promises=0; int k1=1; int j=0;
   for (int i =0; i< domainsize; i++)
     //     {if (index2value(i,var)==val) i++;
     {if (i==val) i++; // pour ne pas renvoyer la valeur courante
     if (i ==  domainsize) break;
     // promises= configuration->get_conflicts_problem(this,var,index2value(i,var));
     promises= ((FullincrCSPConfiguration*)configuration)->tabconflicts[var][i];
     if (promises < minpromises)
       {minpromises = promises; j=i; k1=1;}
     else if (promises == minpromises)
       {k1++;
       if (drand48() < 1/(double)k1) j=i;
       }
     }
   return j;
}










/****************************** COLORIAGE IMPASSE *********************************************/
// seule l'implantation fullincr existe 







void ImpasseColor::move_execution(Configuration* configuration,Move* move)
{ OpProblem::move_execution(configuration,move);
  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  configuration->config[var_changee]=val_changee;
  for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
    {
      if (val_changee == configuration->config[*vi]) 
	configuration->config[*vi]=domainsize;
    }
}

// pour une configuration FullincrCSP  : inutile (herite)
/*
int ImpasseColor::move_evaluation (Configuration * configuration,int valeur,Move* move)
{  
  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;

  int ancienne_val;
  if (sumdegree ==1)
    ancienne_val=     connections[var_changee].size();
  else 
    ancienne_val=1;
  return (valeur + configuration->get_conflicts(var_changee,val_changee,val_changee)- ancienne_val);
}
*/
void ImpasseColor::random_configuration(Configuration* configuration)
{
  for(int j=0;j<nbvar;j++)
    {int indice = (int) (drand48() * variable_domainsize(j));
    configuration->config[j]= tabdomains[domains[j]][indice];
    }
  for (int i=0 ; i<nbvar ; i++)
    for (int j=i+1; j<nbvar; j++)
      if (constraints[i][j] && (configuration->config[i]==configuration->config[j]))
	configuration->config[j]=domainsize;
}



void ImpasseColor::best_config_write()
{CSProblem::best_config_write();}

ImpasseColor* impasse_problem_creation (int nbcol, int degre,ifstream & file)

{ 
  int nbvar; int nbconst;
  int **constraint1; // utilise dans tout csp binaire
  lire_debut_fichier_coloriage(file,nbvar,nbconst); // pour dimensionner les structures
  
  constraint1 =  csp_constraintdatastructure(nbvar);

  
  int* dom = new int[nbvar];
  
  vector<int>* tabdom = new vector<int> [nbvar] ; // les differents types de domaines 
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar];

  lire_fichier_coloriage(file,connex,constraint1);

  ImpasseColor* problem = coloriage_impasse(nbvar,nbconst,nbcol,degre,constraint1);
 // mise en place des domaines et connexions

  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;
}

void ImpasseColor::init_domains(int nbvar, int s)
{for (int i=0;i<nbvar;i++)
  domains[i]=0;
}

void ImpasseColor::init_tabdomains(int s)
{   CSProblem::init_tabdomains(s);
}

void ImpasseColor::compute_var_conflict(Configuration* configuration)
{configuration->var_conflict.clear();
 for (int i=0;i<nbvar;i++)
  {if (configuration->config[i]==domainsize)
    configuration->var_conflict.push_back(i);
  }
}

ImpasseColor* coloriage_impasse(int nbvar,int nbconst,int s,int degree, int** constraint1)
{ImpasseColor* p1 = new ImpasseColor(nbvar,nbconst,s,degree);
p1->constraints=constraint1; 
return p1;
}

void ImpasseColor::best_config_analysis()
{
  int variables_en_conflit =0;
  for (int i =0;i<nbvar;i++)
    if (best_config->config[i]== domainsize)
      variables_en_conflit++;
  *ofile << " nb variables non affectées : " << variables_en_conflit << endl;
}

// arite des variables en conflit si sumdegree=1
int ImpasseColor::config_evaluation (Configuration* configuration)
{int valeur=0; 
 configuration->init_conflicts();
 for (int i=0 ; i< nbvar; i++)
{ 
  if (sumdegree ==1)
    configuration->set_conflicts(i,domainsize,domainsize, connections[i].size());
  else
    configuration->set_conflicts(i,domainsize,domainsize,1);
}

 for(int i=0; i<nbvar; i++)

   {
     // cout << "  " << i << " " << connexions[i].size() << endl;
   if (configuration->config[i]== domainsize)
     {if (sumdegree==1)
       valeur+= connections[i].size();
     else
       valeur++;
     }
   for(int j=i+1; j<nbvar;j++)
     if (constraints[i][j])
	{ if (configuration->config[j]!=domainsize)
              {if (sumdegree==1) configuration->incr_conflicts(i,configuration->config[j], configuration->config[j], connections[j].size());
	      else configuration->incr_conflicts(i,configuration->config[j],configuration->config[j],1);}
	 if (configuration->config[i]!=domainsize)
              {if (sumdegree==1) configuration->incr_conflicts(j,configuration->config[i],configuration->config[i],connections[i].size());
	      else configuration->incr_conflicts(j,configuration->config[i],configuration->config[i],1);
	      }
	}
   }
 return valeur;
}

ImpasseColor::ImpasseColor (int nvar, int nconst, int nbcolor, int degree) :
ColorCSProblem (nvar,nconst,nbcolor){sumdegree=degree; domainsize=nbcolor; }


void ImpasseColor::fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move)
{
  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;

  configuration->set_conflicts(var_changee,val_changee,val_changee,0);
  for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
   {
     if (sumdegree==1)
       configuration->incr_conflicts(*vi,val_changee,val_changee,connections[var_changee].size());
     else
       configuration->incr_conflicts(*vi,val_changee,val_changee,1);
     if (configuration->config[*vi] == val_changee)
       for (vector<int>::iterator wi= connections[*vi].begin();wi!= connections[*vi].end(); wi++)
	 if (*wi != var_changee)
	   {if (sumdegree==1)
	     configuration->incr_conflicts(*wi,val_changee,val_changee,-(connections[*vi].size()));
	   else
	     configuration->incr_conflicts(*wi,val_changee,val_changee,-1);
	   }
   }
}
