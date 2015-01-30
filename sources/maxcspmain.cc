#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;
#include <iostream>
#include <fstream>
#include "timer.h"
#include "incop.h"
#include "csproblem.h"
#include "incoputil.h"

#include "extcsp.h"


#include <math.h>
#include <unistd.h>

/** le lecteur des fichiers MAX-CSP au format ds de Simon de Givry
    pour le moment, ne lit que des problèmes avec contraintes binaires en extension , les valeurs des domaines
doivent être des entiers.
Dans cette version Max-CSP, chaque contrainte a cout 1 et chaque couple de variable a au plus une contrainte.
Quelques limitations dues à l'utilisation de tableaux (nb variables, nb contraintes, nb d'instances de problèmes
*/

extern ofstream* ofile;  // le fichier de sortie

extern Stat_GWW * Statistiques;



ExtensionBinaryCSProblem* weighted_csp1(int nbvar, int nbconst, int s, int ** constraint1, int*** constraint2, vector<int>& wei)
{ExtensionBinaryCSProblem*  p1 =new  ExtensionBinaryCSProblem (nbvar,nbconst,s);
// p1->cstweights=wei;
 p1->constraints=constraint1;
 p1->constraint2=constraint2;
 return p1;
}

ExtensionBinaryCSProblem* weighted_csp_creation1 (int nbvar, int nbconst, int s, int ** constraint1, int*** constraint2,  vector<int>* connexions, vector<int>& wei)
{
  ExtensionBinaryCSProblem* problem = weighted_csp1 (nbvar,nbconst,s,constraint1,constraint2,wei);
  extensionbinary_setconnexions(nbvar,connexions,constraint1);
  return problem;
}







/** lecture du debut du fichier : le probleme et les variables */
void  dsdata_file_read (ifstream & file, int& nbvar,int& tabu, vector<string> & variable_names, vector<int>* tabdomaines)
{char cc[256];
 string tt;
 char* tt0[256];
 int nb=0;
 file >> tt; // nom du  probleme
 file >> tt;
 if ((tt == "tabu") || (tt =="tabu_valued"))
   tabu=1;
 if ((tt== "classic") || (tt == "valued") || (tt == "tabu") || (tt == "tabu_valued") || (tt == " tabu_classic"))
   file >> tt;
 while (tt != "#")
   {variable_names.push_back(tt);
    file.getline(cc,255);
    //    cout << cc << endl;
    tt0[0]=(char*)strtok(cc," ");
    //    cout << tt0[0] << endl;
    tabdomaines[nb].push_back(atoi(tt0[0]));
    int i=1;
    while (tt0[i-1]!= NULL)
      {tt0[i]=(char*)strtok(NULL," ");
      //cout << tt0[i] << endl;
      if (tt0[i] != NULL)
	tabdomaines[nb].push_back(atoi(tt0[i]));
      i++;}
   nb++;
   file >> tt;}
 nbvar=nb;

 }

/* utilitaire : recherche la place d'un nombre dans un vecteur */
int value_number(int value, vector<int>& tabdomaine)
{ for (int i=0 ; i< (int)tabdomaine.size(); i++)
  {if (tabdomaine[i]==value)
    return i;
  }
return -1;
}

/* utilitaire : recherche la place d'une chaine dans un vecteur */
int string_number(string& value, vector<string>& variablename)
{ for (int i=0 ; i< (int) variablename.size(); i++)
  {if (variablename[i]==value)
    return i;
  }
return -1;
}

/** lecture des contraintes */
void  dsdata_constraint_read (ifstream & file, int& nbconst,vector<string> & variable_names, vector<string> & constraint_names,
vector<int>& variable1, vector<int>& variable2, int tabu,
int*** constraint2,   vector<int> * tabdomaines, vector<int>& weight)
{
  string tt;
  string tt1; int n1;
  string tt2; int n2;
  int nval ; int nval1;
  file >> tt;
  int nc=0;
  while(tt != "%")
    {constraint_names.push_back(tt);
    file >> tt;
    if (tt != "extension")
      {weight.push_back(atoi( tt.c_str())); file >> tt;}
    else
      weight.push_back(1);

    file >> tt1;
    file >> tt2;
    n1 = string_number(tt1, variable_names);
    n2 = string_number(tt2, variable_names);
    if (n1 < n2)
      { variable1.push_back(n1);
      variable2.push_back(n2);}
    else
      { variable1.push_back(n2);
      variable2.push_back(n1);}
    file >> tt ;
    while ((tt != "#") && (tt != "%"))
      {file >> tt1;

      if (n1 < n2)
	{nval=value_number(atoi(tt.c_str()),tabdomaines[n1]);
	nval1=value_number(atoi(tt1.c_str()),tabdomaines[n2]);}
      else
	{nval=value_number(atoi(tt1.c_str()),tabdomaines[n2]);
	nval1=value_number(atoi(tt.c_str()),tabdomaines[n1]);}
      if (tabu) constraint2[nc][nval][nval1]=1;
      else constraint2[nc][nval][nval1]=0;
      file >> tt;}
    nc++;
    if (tt != "%") file >> tt;
    }
}



/** lecture des instances : quelles contraintes sont dans le probleme : remplissage du tableau constraint1 */
void dsdata_instance_read(ifstream& file, int& nbconst,vector<int>& variable1, vector<int>& variable2, int** constraint1,
			  vector<string> & constraint_names)
{
  nbconst=0;
  string tt;
  file >> tt;  // le numero de l'instance
  file >> tt;  // la 1re contrainte
  while ((tt != "%") && (! file.eof()))
    {
    int nc1= string_number(tt,constraint_names);
    int nv1 = variable1[nc1];
    int nv2 = variable2[nc1];
    constraint1[nv1][nv2]=nc1+1;
    nbconst++;
    file >> tt;
    }

}


vector<int>** wextcsp_constraintdatastructure(int nbvar)
{vector<int>** constraint1= new vector<int>*[nbvar];
  for(int i=0;i<nbvar;i++)
    {constraint1[i]=new vector<int>[nbvar];}
  return constraint1;
}

/** la fonction principale */
int main (int argc, char** argv) {

  ExtensionBinaryCSProblem* problem ;          // pointeur sur le probleme

  // les divers arguments lus dans la ligne de commande
  int nbvar,nbconst;
  int taille,nbessais;
  int graine1;
  int narg = 2;  // compteur des arguments

  // le nom du fichier de sortie : pour les tests : version logiciel + concaténation des arguments
  char filename [1000];
  if ((string)argv[1] == "arg")
    ofile_name(filename, argc, argv);
  else sprintf(filename,"%s",argv[1]);

  ofstream ofile1 (filename);
  ofile = & ofile1;
  ifstream file (argv[2]); // le fichier de données

  // les limites en dur a cause de predimensionnement de tableaux ( a changer avec des vecteurs ??)
  int MAXVAR=500;  // nb max de variables
  int MAXCONST=1000;  // nb max de contraintes
  int NPBMAX = 1000;  // nb max de problemes


  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (NPBMAX, nbessais);


  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la recuperation du signal 10
  sigaction();

  // Declaration des variables contenant les structures de données des problemes
  int **constraint1;
  int*** constraint2;
  vector<int> tabdomaines[MAXVAR] ; // les différents types de domaines - LIMITATION aux 500 variables
  // Initialisation des structures de données des problèmes
  vector<int> connexions[MAXVAR];
  vector<string> variable_names;
  int tabu=0;
  dsdata_file_read (file,nbvar,tabu, variable_names, tabdomaines);
  int domaines[nbvar];  // 1 domaine par variable
  for(int i=0;i<nbvar;i++)
    {domaines[i]=i;}

  constraint1 = csp_constraintdatastructure(nbvar);
      /* Initialisation des tableaux */

  int maxdomsize = 0;
  for (int i=0; i<nbvar;i++)
    if (maxdomsize < (int) tabdomaines[i].size()) maxdomsize=tabdomaines[i].size();
  constraint2 = extensionbinary_tupledatastructure (MAXCONST,maxdomsize); // limitation a 1000 contraintes
  extensionbinary_inittuples(MAXCONST,maxdomsize,constraint2,tabu);
  vector<int> weight;
  vector<string> constraint_names;
  vector<int> variable1;
  vector<int> variable2;
  dsdata_constraint_read (file, nbconst, variable_names, constraint_names, variable1, variable2, tabu, constraint2,   tabdomaines,  weight);
  int pbnumber=0;
  while (! file.eof())
    { Statistiques->init_pb(pbnumber);
    for(int i=0;i<nbvar;i++)
      for(int j=0;j<nbvar;j++)
	constraint1[i][j]=0;
    dsdata_instance_read(file,nbconst,variable1,variable2,constraint1,constraint_names);
    problem = weighted_csp_creation1 (nbvar,nbconst,maxdomsize,constraint1,constraint2, connexions, weight);


    // mise en place des domaines et connexions
    problem->set_domains_connections(domaines,tabdomaines,connexions);
    for (int i=0;i<nbvar;i++)
      {*ofile << "variable  " << i ;
	for (int k=0; k< (int) problem->connections[i].size(); k++)
	  *ofile << " " << problem->connections[i][k] ;
	*ofile << endl;}
    // creation de la population et initialisation
    // La population : tableau de configurations
    Configuration* population[taille];
    problem->init_population(population,taille);

    problem->allocate_moves();

    // boucle sur les essais
    *ofile << " pb numéro " << pbnumber;
    for(int nessai = 0;nessai< nbessais ; nessai++)
      executer_essai (problem,algo,population,taille,graine1,nessai);

    // ecriture statistiques
    Statistiques->current_try++;
    ecriture_stat_probleme();
    delete problem;
    pbnumber++;}
  if (pbnumber > 1)
    ecriture_statistiques_global ();

  cout << "Fin résolution " << Statistiques->total_execution_time << endl;
  return 0;


}



