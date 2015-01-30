#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <iostream>
using namespace std;
#include <fstream>
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "move.h"
#include "carseq.h"
#include "autotuning2.h"

extern ofstream* ofile;  // le fichier de sortie


extern Stat_GWW * Statistiques; // trombe_ajout: l'objet pour les stats en var globale


// le problème de l'ordonnancement de voitures - version evaluation incrementale d'un mouvement d'échange
// le cout d'une séquence pour une option est le nombre de voitures en trop pour cette option dans la séquence.

int carseq(int argc, char** argv,int tuningmode)
{

  // les divers arguments lus dans la ligne de commande
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


  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (1, nbessais);

  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la récupération du signal 10

  sigaction();
  double maxtime;
  if (tuningmode)
    arguments_tempscpu (argv,narg,maxtime);
  // creation du probleme (lecture des données, création des structures de données et du problème)
  Carseq* problem = carseqproblem (file);
  if ((string)argv[2]=="pb6-76") problem->lower_bound = 6;
  if ((string)argv[2]=="pb19-71") problem->lower_bound = 2;
  if ((string)argv[2]=="pb21-90") problem->lower_bound = 2;
  if ((string)argv[2]=="pb36-92") problem->lower_bound = 2;
  if ((string)argv[2]=="pb10-93") problem->lower_bound = 3;
  // creation de la population et initialisation
  // La population : tableau de configurations
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->allocate_moves();
  // initialisation des statistiques
  Statistiques->init_pb(0);

  if (tuningmode)
    // réglage automatique
    {int initwalklength=1000000;
   autosolving((LSAlgorithm*)algo,population,problem,0,graine1,nbessais,maxtime,initwalklength);
    }
  else
    // boucle sur les essais

    {for(int nessai = 0;nessai< nbessais ; nessai++)
      executer_essai (problem,algo,population,taille,graine1,nessai);

    // ecriture statistiques

    Statistiques->current_try++;
    ecriture_stat_probleme();

    }

  delete problem;
  std::cout << "Fin résolution " << Statistiques->total_execution_time << std::endl;
  return 0;


}





// lecture du fichier au format CSPLIB
Carseq* carseqproblem(ifstream& file)

{
 int nbcars; int nbopt; int nbclasses;
 file >> nbcars;
 file >> nbopt;
 file >> nbclasses;
 *ofile << nbcars << " " << nbopt << " " << nbclasses << endl;
 int* maxcars = new int [nbopt];
 int* blocsize = new int [nbopt];
 for (int i = 0; i< nbopt;i++)
   file >> maxcars[i];
 for (int i = 0; i< nbopt;i++)
   file >> blocsize[i];

 int* nbcarsinclass = new int [nbclasses];
 int** optionsinclass = new int*[nbclasses];
 for (int i=0 ; i< nbclasses;  i++)
   {optionsinclass[i]= new int[nbopt];
   int jj; file >> jj; file >> nbcarsinclass[i];
   for (int j = 0; j<nbopt; j++)
     file >> optionsinclass[i][j];
   }
 Carseq* problem= new Carseq(nbcars,nbclasses);
 int* dom = new int[nbcars];
 vector<int>* tabdom = new vector<int> [nbcars] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbcars];

 problem->set_domains_connections(dom,tabdom,connex);
 problem->nbopt=nbopt;
 problem->nbclasses=nbclasses;
 problem->maxcars=maxcars;
 problem->blocsize=blocsize;
 int* maxblocsize= new int[nbclasses];
 int blocsizemax=0;
 for (int i=0; i< nbclasses; i++)
   {int blocsi=0;
   for (int k=0;k<nbopt;k++)
     {if ((optionsinclass[i][k]) &&  (blocsize[k]>blocsi))
       blocsi=blocsize[k];
     }
   maxblocsize[i]=blocsi;
   if (blocsi > blocsizemax) blocsizemax=blocsi;}
 *ofile << " maxblocsize " ;
 for (int i=0;i<nbclasses;i++)
   *ofile << " " << maxblocsize[i] ;
 *ofile << endl;
 problem->nbcarsinclass=nbcarsinclass;
 problem->maxblocsize=maxblocsize;
 problem->blocsizemax=blocsizemax;
 problem->optionsinclass=optionsinclass;

 problem->init_domain_tabdomain();
 // *ofile << " probleme entré " <<  nbcars << " voitures " << nbclasses << " classes " << endl;

 return problem;
}

Carseq::Carseq (int nbcars,int nbclasses) :
  CSProblem (nbcars,1){domainsize=nbclasses;}

CarseqConfiguration::CarseqConfiguration(int nbvar,int nbclasses) :
  Configuration(nbvar){nbcarclasses=nbclasses;  trynumber=0;
  tabconflicts = new int*[nbvar];
      	for(int i=0;i<nbvar;i++)
	  {
	    tabconflicts[i]= new int[nbclasses];
	    for (int j=0;j<nbclasses;j++)
	      tabconflicts[i][j]=0;
	  }
}



CarseqConfiguration::~CarseqConfiguration()
{
;}

void CarseqConfiguration::copy_element ( Configuration* config2)
{ Configuration::copy_element (config2);
 for (int i=0; i< nbvar ; i++)
   for (int j=0; j< nbcarclasses;j++)
   {tabconflicts[i][j]= ((CarseqConfiguration*)config2)->tabconflicts[i][j];
   }
}


CarseqMove::CarseqMove(int nbopt1, int nbvar1) :
  SwapMove(){nbopt=nbopt1;
  nbvar=nbvar1;
}

CarseqMove::~CarseqMove()
{
  ;}


void CarseqConfiguration::init_conflicts()
  {for(int i=0;i<nbvar;i++)
    for (int j=0;j<nbcarclasses;j++)
      tabconflicts[i][j]=0;
  }

int CarseqMove::blocsizemax(Configuration* configuration, Carseq* problem)
{if
  (problem->maxblocsize[configuration->config[variable1]] > problem->maxblocsize[configuration->config[variable2]])
  return (problem->maxblocsize[configuration->config[variable1]]);
 else
   return (problem->maxblocsize[configuration->config[variable2]]);
}

Configuration* Carseq::create_configuration()
{return ((Configuration*)new CarseqConfiguration(nbvar,nbclasses));}

int Carseq::config_evaluation (Configuration* configuration)
{int valeur=0;
 configuration->init_conflicts();
 for (int i=0; i< nbopt; i++)
   {
   for (int j=0; j+blocsize[i]-1 < nbvar; j++)
     {
       int nbcarsopt=0;
       for (int k=0; k< blocsize[i] ; k++)
	 if (optionsinclass[configuration->config[k+j]][i])
	   nbcarsopt++;
       if (nbcarsopt > maxcars[i])
	 {
	   valeur+= nbcarsopt-maxcars[i];}
     }

   //           *ofile << " cout apres verif option " << i << " " << maxcars[i] << " " << blocsize[i] << " " << valeur << endl;
   }
 for (int j=0; j< nbvar; j++)
   for(int k=0; k< nbclasses; k++)
     ((CarseqConfiguration*)configuration)->tabconflicts[j][k]=partialinitconflicts(configuration,j,k);
 return valeur;
}

// pour debogage
int Carseq::config_verification (Configuration* configuration)
{int valeur=0;
 for (int i=0; i< nbopt; i++)
   {
   for (int j=0; j+blocsize[i]-1 < nbvar; j++)
     {
       int nbcarsopt=0;
       for (int k=0; k< blocsize[i] ; k++)
	 if (optionsinclass[configuration->config[k+j]][i])
	   nbcarsopt++;
       if (nbcarsopt > maxcars[i])
	 {
	   valeur+= nbcarsopt-maxcars[i];}
     }

   //           *ofile << " cout apres verif option " << i << " " << maxcars[i] << " " << blocsize[i] << " " << valeur << endl;
   }
 for (int j=0; j< nbvar; j++)
   for(int k=0; k< nbclasses; k++)
     { int val = partialinitconflicts(configuration,j,k);
     if (val != ((CarseqConfiguration*)configuration)->tabconflicts[j][k])
       *ofile << " Erreur  : variable " << j << " classe " << k  << " valeurs " << val << " " <<
	 ((CarseqConfiguration*)configuration)->tabconflicts[j][k] << endl;
     ((CarseqConfiguration*)configuration)->tabconflicts[j][k]=partialinitconflicts(configuration,j,k);
     }
 return valeur;
}

void Carseq::best_config_verification()
{
  OpProblem::best_config_verification();
}



/* une permutation aleatoire */

void Carseq :: random_configuration(Configuration* configuration)
{//   *ofile << "debut random configuration " << endl;
 list<int> not_used; int placed=0;
 for (int j=0;j<nbvar;j++)
   { not_used.push_back(j);}
 for (int k=0; k<nbclasses ; k++)
   {// *ofile << "classe k " << k << " nb voitures " << nbcarsinclass[k] << endl;
   for (int l=0; l<nbcarsinclass[k]; l++)
     {//int indice = (int) (drand48() * (nbvar-placed));
     int indice = rand() % (nbvar-placed);
     int jj =0; int val;
     for (list<int>::iterator it = not_used.begin(); it != not_used.end(); it++)
       {if (jj == indice)
	 {val=*it; break;}
       jj++;}
     not_used.remove(val);
     // *ofile << " val " << val << " k " << k << " l " << l << endl;
     configuration->config[val]= k;
     placed++;
     }
   }
 // *ofile << "fin random configuration " << endl;
 // for (int i =0 ; i<nbvar; i++)
 //   {*ofile << configuration->config[i] << " " ;}
 // *ofile << endl;
}


int Carseq::nbconflicts_partialconfig(Configuration* configuration,int length, int carclass)
{int valeur=0;

 for (int i=0; i< nbopt; i++)

   {if ((optionsinclass[carclass][i]) && (length-blocsize[i]+1 >=0))
     {
       int nbcarsopt=1;
       for (int k=0 ; k< blocsize[i]-1 ; k++)
	 if (optionsinclass[configuration->config[k+length-blocsize[i]+1]][i])
	   nbcarsopt++;
       if (nbcarsopt > maxcars[i])
	 {
	   valeur++;}
     }
   }
 return valeur;
}



// configuration initiale gloutonne  : variante non concluante (meilleure configuration initiale, mais plus difficile
// à améliorer)
/*
void Carseq :: random_configuration(Configuration* configuration)
{
 int nbcarstoplace[nbclasses];
 for (int i =0; i< nbclasses; i++)
   nbcarstoplace[i]=nbcarsinclass[i];
 for (int j=0; j<nbvar; j++)
   {int nbconflicts=RAND_MAX ; int ii=0; int k1=1;
      for (int i =0;i<nbclasses;i++)
	{if (nbcarstoplace[i])
	  {int nbconf = nbconflicts_partialconfig(configuration,j,i);
	  if (nbconf < nbconflicts)
	    {ii=i;k1=1; nbconflicts=nbconf;}
	  else if (nbconf==nbconflicts)
	    {k1++;
	    if (drand48() < 1/(double)k1) ii=i;
	    }
	  }
	}
      configuration->config[j]=ii;
      nbcarstoplace[ii]--;
   }
}
*/

Move* Carseq::create_move()
{CarseqMove* move = new CarseqMove(nbopt,nbvar);
 return (Move*)move;
}


// 2 variantes :  tirage aléatoire dans les variables en conflit
//                tour de role dans les variables en conflit

int Carseq::random_conflict_variable(Configuration* configuration)
{// *ofile << "VC " << configuration->var_conflict.size() << endl;
  // return configuration->var_conflict[(int) (drand48() * configuration->var_conflict.size())];
  //  return configuration->var_conflict[rand() % configuration->var_conflict.size()];
  ((CarseqConfiguration*)configuration)->trynumber++;
 return configuration->var_conflict[((CarseqConfiguration*)configuration)->trynumber % configuration->var_conflict.size()];
}


/* un mouvement de permutation de deux valeurs */

void Carseq::next_move
(Configuration* configuration, Move* move, NeighborhoodSearch* nbhs)
{
 int var1; int var2;
 if (nbhs->var_conflict)
   {var1 = Carseq::random_conflict_variable(configuration);}
 else
   var1=random_variable(configuration);
 ((SwapMove*)move)->variable1 = var1;


 if (nbhs->val_conflict)   // Minton
   {var2 = chose_bestpermutation(configuration,(CarseqMove*)move);}
 else
   {//var2 = (int) (drand48 () * (nbvar-1));   // un echange quelconque
     //      var2 = ((var1 * nbvar ) + ((CarseqConfiguration*)configuration)->trynumber) % (nbvar - 1);
     var2 = rand() % (nbvar -1);
     if (var2 >= var1) var2++;   // pour que var2 soit different de var1
      // essai non concluant de biaiser la 2eme variable
     //     var2 = rand() % (nbvar -1  + configuration->var_conflict.size() -1 );
     //     if (var2 >= nbvar-1)
     //    var2 = configuration->var_conflict[var2-nbvar-1];

 // var2=var1+1;                        // deux variables consecutives
      int var2found=0;
      for (int indvar2=var2; indvar2<nbvar; indvar2++)
	if (configuration->config[indvar2] != configuration->config[var1])
	  { var2=indvar2; var2found=1; break;}
      if (!var2found)
	for (int indvar2=0; indvar2<var2; indvar2++)
	  if (configuration->config[indvar2] != configuration->config[var1])
	    { var2=indvar2; // var2found=1;
	    break;}
   }
 ((SwapMove*)move)->variable2 = var2;
 move->valuation = Carseq::move_evaluation (configuration,move);
}

// pour voisinage a la min-conflits :  la meilleure position  pour var2 sur une fenetre de longueur nbvar/xx
int Carseq::chose_bestpermutation(Configuration* configuration, CarseqMove* move)
{int var1= move->variable1;
 int nbconflicts=RAND_MAX; int k1=1; int var=-1;
 // int var22 = (int) (drand48 () * (nbvar-(nbvar/10)));   // le meilleur echange sur nbvar/xx  (pour pbmes base carseq_500_20_8 : 40 semble bien)
 int var22 = rand() % (nbvar-(nbvar/10));
 for (int var2 =var22; var2< var22 + nbvar/10; var2++)
   { if (configuration->config[var2] != configuration->config[var1])
     {move->variable2 = var2;
      int nbconf= move_evaluation(configuration,move);
      if (nbconf < nbconflicts) {var=var2; nbconflicts=nbconf; k1=1;}
      else if (nbconf==nbconflicts)
	    {k1++;
	    if (drand48() < 1/(double)k1) var=var2;
	    }
     }
   }
 if (var == -1)
   return var1;
 else return var;
}





/*
int Carseq::random_conflict_variable(Configuration* configuration)
{
 int var1=random_variable(configuration);
 for (int var =var1; var < nbvar; var++)
   if (((CarseqConfiguration*)configuration)->tabconflicts[var][configuration->config[var]])
     {
       return var;}
 for (int var =0 ; var < var1 ; var++)
   if (((CarseqConfiguration*)configuration)->tabconflicts[var][configuration->config[var]])
     {return var;}
 return var1;
}
*/

int Carseq::move_evaluation (Configuration* configuration,Move* move)
{  int var1 = ((SwapMove*) move)->variable1;
   int var2 = ((SwapMove*) move)->variable2;
   int blocsizemax1= ((CarseqMove*)move)->blocsizemax(configuration,this);
   int valeur=configuration->valuation;
      if (((var2-var1) >= blocsizemax1) || ((var1-var2) >= blocsizemax1)){
	//  if (((var2-var1) >= blocsizemax) || ((var1-var2) >= blocsizemax)){
     valeur+=
       - ((CarseqConfiguration*)configuration)->tabconflicts[var1][configuration->config[var1]]
       - ((CarseqConfiguration*)configuration)->tabconflicts[var2][configuration->config[var2]]
       + ((CarseqConfiguration*)configuration)->tabconflicts[var1][configuration->config[var2]]
       + ((CarseqConfiguration*)configuration)->tabconflicts[var2][configuration->config[var1]] ;
   }

   else
     {

       int val1= configuration->config[var1];
       int val2= configuration->config[var2];
       configuration->config[var1]=val2;
       configuration->config[var2]=val1;
       int conf12 = partialinitconflicts(configuration,var1,val2);
       int conf21 = partialinitconflicts(configuration,var2,val1);
       valeur +=
	 - ((CarseqConfiguration*)configuration)->tabconflicts[var1][val1]
	 - ((CarseqConfiguration*)configuration)->tabconflicts[var2][val2]
	 + conf12 + conf21;

       configuration->config[var1]=val1;
       configuration->config[var2]=val2;
     }
   return valeur;

}

int Carseq::partialconflicts(Configuration* configuration, int var1, int val2)
{int valeur=0;
 for (int i=0; i<nbopt; i++)
   {if (optionsinclass[val2][i])
     {
     int j= var1-blocsize[i]+1; if (j<0) j=0;
     for (int k=j; k< var1+1  && k  + blocsize[i] -1 < nbvar; k++)
       {int nbcarsopt=0;
       for (int k1=0; k1< blocsize[i]; k1++)
	 if (optionsinclass[configuration->config[k+k1]][i] )
	   nbcarsopt++;

       if (nbcarsopt > maxcars[i])
	 {valeur++;
	 }
       }
     }

   }
 return valeur;
}

int Carseq::partialinitconflicts(Configuration* configuration, int var1, int val2)
{int valeur=0;
 int val1= configuration->config[var1];
 configuration->config[var1]=val2;
for (int i=0; i<nbopt; i++)
   {if (optionsinclass[val2][i])
     {
     int j= var1-blocsize[i]+1; if (j<0) j=0;
     for (int k=j; k< var1+1  && k  + blocsize[i] -1 < nbvar; k++)
       {int nbcarsopt=0;
       for (int k1=0; k1< blocsize[i]; k1++)
	 if (optionsinclass[configuration->config[k+k1]][i] )
	   nbcarsopt++;

       if (nbcarsopt > maxcars[i])
	 {valeur++;
	 }
       }
     }

   }
 configuration->config[var1]=val1;
 return valeur;
}


/*
int Carseq::partialconflicts(Configuration* configuration, int var1, int var2)
{int valeur=0;
 int val1 = configuration->config[var1];
 int val2 = configuration->config[var2];
 configuration->config[var2]=val1;
 configuration->config[var1]=val2;
 // *ofile << " var1 " << var1 <<  " val1 " << val1 << " var2 " << var2 << " val2 " << val2 << endl ;
 for (int i=0; i<nbopt; i++)
   {if (optionsinclass[val2][i])
     {// *ofile << "option " << i << " maxcars " << maxcars[i] << " blocsize " << blocsize[i] << endl;
     int j= var1-blocsize[i]+1; if (j<0) j=0;
     for (int k=j; k< var1+1  && k  + blocsize[i] -1 < nbvar; k++)
       {int nbcarsopt=0;
       for (int k1=0; k1< blocsize[i]; k1++)
	 if (optionsinclass[configuration->config[k+k1]][i] )
	   nbcarsopt++;
       // * ofile << " option " << i << " nbcarsopt " << nbcarsopt << endl;
       if (nbcarsopt > maxcars[i])
	 {valeur++; // *ofile << var1 << "  " << var2 <<  " " << i << "  " << valeur << endl;
	 }
       }
     }
   }
 configuration->config[var2]=val2;
 configuration->config[var1]=val1;
 return valeur;
}
*/

/*
// version recalcul
void Carseq::update_conflicting_variables (Configuration* configuration,Move* move)
{ int var1= ((SwapMove*) move)->variable1;
  int var2 = ((SwapMove*) move)->variable2;
  int val1 = configuration->config[var1];
  int val2 = configuration->config[var2];
  int blocsizemax= ((CarseqMove*)move)->blocsizemax(configuration,this);
  configuration->config[var1]=val2;
  configuration->config[var2]=val1;

  for (int i=0; i<nbvar; i++)
    {if ((i > var1- blocsizemax && i < var1 + blocsizemax )
	 ||(i > var2- blocsizemax && i < var2 + blocsizemax ))
      for (int k=0; k<nbclasses ; k++)
	((CarseqConfiguration*)configuration)->tabconflicts[i][k] = partialinitconflicts(configuration, i, k);
	}
  configuration->config[var1]=val1;
  configuration->config[var2]=val2;
}
*/


// mise a jour incrementale de la structure de données des variables en conflit avant d'effectuer le mouvement move
void Carseq::update_conflicting_variables (Configuration* configuration,Move* move)
{


  int var1 = ((SwapMove*) move)->variable1;
  int var2 = ((SwapMove*) move)->variable2;

  //*ofile << " mvt : var1 "  << var1 << " " <<
  //   configuration->config[var1] <<  " var2 "  << var2 << " " <<
  //   configuration->config[var2] << endl;

  int var3=var1;
  if (var1 > var2) {var1=var2; var2=var3;}
  for (int i=0; i<nbopt; i++)
    {
      if (optionsinclass[configuration->config[var1]][i]&&
	  ! optionsinclass[configuration->config[var2]][i])
	{
	  int j= var1-blocsize[i]+1; if (j<0) j=0;
	  for (int k=j; k< var1+1  && k  < var2- blocsize[i] +1; k++)
	    {int nbcarsopt=0;
	    for (int k1=0; k1< blocsize[i]; k1++)
	      if (optionsinclass[configuration->config[k+k1]][i])
		nbcarsopt++;
	    if (nbcarsopt == maxcars[i]+1)
	      {
		for (int k1=0; k1< blocsize[i]; k1++)
                  if (optionsinclass[configuration->config[k+k1]][i] && k+k1 != var1)
		    for (int cl=0; cl< nbclasses; cl++)
		      { if (optionsinclass[cl][i])
			{
			  ((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]--;
			}
		      }
	      }
	    if (nbcarsopt == maxcars[i])
	      {
		for (int k1=0; k1< blocsize[i]; k1++)
		  if (!optionsinclass[configuration->config[k+k1]][i])
		    for (int cl=0; cl< nbclasses; cl++)
		      { if (optionsinclass[cl][i])
		      {
			((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]--;
		      }
		    }
	      }
	    }
        j=var2-blocsize[i]+1; if (j<0) j=0;
	if (var2-blocsize[i] <var1 )
	  {
	    for (int k=j ; k<var1+1 && k + blocsize[i] < nbvar +1; k++)
	      {int nbcarsopt=0;
	      for (int k1=0; k1< blocsize[i]; k1++)
		{if (optionsinclass[configuration->config[k+k1]][i])
		  nbcarsopt++;}
	      if (nbcarsopt == maxcars[i])
		{for (int cl =0;cl < nbclasses; cl++)
		  {if (optionsinclass[cl][i])
		    {((CarseqConfiguration*)configuration)->tabconflicts[var1][cl]++;
		    ((CarseqConfiguration*)configuration)->tabconflicts[var2][cl]--;
		    }

		  }
		}
	      }
	    j=var1+1;
	  }

	for (int k=j; k< var2+1  && k + blocsize[i] < nbvar +1 ; k++)
	  {int nbcarsopt=0;
	  for (int k1=0; k1< blocsize[i]; k1++)
	    if (optionsinclass[configuration->config[k+k1]][i])
	      nbcarsopt++;
	  if (nbcarsopt == maxcars[i])
	    {
	      for (int k1=0; k1< blocsize[i]; k1++)
		if (optionsinclass[configuration->config[k+k1]][i])
		  for (int cl=0; cl< nbclasses; cl++)
		    if (optionsinclass[cl][i])
		      {
			((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]++;
		      }

	    }
	  if (nbcarsopt == maxcars[i]-1 )
	    {
	      for (int k1=0; k1< blocsize[i]; k1++)
		if (!(optionsinclass[configuration->config[k+k1]][i]) && k+k1 != var2 )
		  for (int cl=0; cl< nbclasses; cl++)

		  if (optionsinclass[cl][i])
		      {
		    ((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]++;
		      }
	    }

	  }
	}
      else if
	(optionsinclass[configuration->config[var2]][i]&&
	 ! optionsinclass[configuration->config[var1]][i])
	{int j= var1-blocsize[i]+1; if (j<0) j=0;
	for (int k=j; k< var1+1  && k < var2- blocsize[i] +1; k++)
	  {int nbcarsopt=0;
	  for (int k1=0; k1< blocsize[i]; k1++)
	    if (optionsinclass[configuration->config[k+k1]][i])
	      nbcarsopt++;
	  if (nbcarsopt == maxcars[i])
	    {
	      for (int k1=0; k1< blocsize[i]; k1++)
		if (optionsinclass[configuration->config[k+k1]][i])
		  for (int cl=0; cl< nbclasses; cl++)
		    if (optionsinclass[cl][i])

		      {

			((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]++;
		      }

	    }

	  if (nbcarsopt == maxcars[i]-1)
	    {
	      for (int k1=0; k1< blocsize[i]; k1++)
		if  (!(optionsinclass[configuration->config[k+k1]][i]) && (k+k1 != var1))
		for (int cl=0; cl< nbclasses; cl++)
		  if (optionsinclass[cl][i])
		      {
		    ((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]++;
		      }

	    }

	  }
	j=var2-blocsize[i]+1;  if (j<0) j=0;
	if (var2-blocsize[i] <var1 )
	  {

	    for (int k=j ; k<var1+1 && k + blocsize[i] < nbvar +1; k++)
	      {int nbcarsopt=0;
	      for (int k1=0; k1< blocsize[i]; k1++)
		{if (optionsinclass[configuration->config[k+k1]][i])
		  nbcarsopt++;}
	      if (nbcarsopt == maxcars[i])
		{for (int cl =0;cl < nbclasses; cl++)
		  {if (optionsinclass[cl][i])
		    {((CarseqConfiguration*)configuration)->tabconflicts[var2][cl]++;
		    ((CarseqConfiguration*)configuration)->tabconflicts[var1][cl]--;}
		  }
		}
	      }

	    j=var1+1;
	  }

	for (int k=j; k< var2+1  && k + blocsize[i] < nbvar +1; k++)
	  {int nbcarsopt=0;
	  for (int k1=0; k1< blocsize[i]; k1++)
	    if (optionsinclass[configuration->config[k+k1]][i])
	      nbcarsopt++;
	  if (nbcarsopt == maxcars[i]+1)
	    {
	      for (int k1=0; k1< blocsize[i]; k1++)
		if ((optionsinclass[configuration->config[k+k1]][i]) && (k+k1 != var2))
		for (int cl=0; cl<  nbclasses; cl++)
		  if (optionsinclass[cl][i])
		    {
		    ((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]--;
		    }
	    }
	  if (nbcarsopt == maxcars[i])
	    {
	      for (int k1=0; k1< blocsize[i]; k1++)
		if (!optionsinclass[configuration->config[k+k1]][i])
		for (int cl=0; cl<  nbclasses; cl++)
		  if (optionsinclass[cl][i])
		    {
		    ((CarseqConfiguration*)configuration)->tabconflicts[k+k1][cl]--;
		    }

	    }

	  }

	}
    }
}


void Carseq::move_execution(Configuration* configuration,Move* move)
{update_conflicting_variables (configuration,move);
 OpProblem::move_execution(configuration,move);
 int aval = configuration->config[((SwapMove*) move)->variable1];
 configuration->config[((SwapMove*) move)->variable1]=configuration->config[((SwapMove*) move)->variable2];
 configuration->config[((SwapMove*) move)->variable2]=aval;
 // *ofile << " mvt effectué " << " var 1 " << ((SwapMove*) move)->variable1 << " var 2 " << ((SwapMove*) move)->variable2 << endl;
 // int value = configuration->valuation;
 // int value1= config_verification(configuration);//   pour debogage
 // if (value != value1) *ofile << " Erreur evaluation " << value << " au lieu de : " << value1 << endl;
}



/* calcul des variables en conflit : on determine si une variable est en conflit pour une option.
On utilise la structure de données tabconflicts . on met chaque variable en conflit dans la liste autant de fois
que son nombre de conflits courant)*/


void Carseq::compute_var_conflict (Configuration* configuration)
{configuration->var_conflict.clear();
 configuration->var_conflict_size=0;
 for(int j=0;j<nbvar;j++)
      {if (((CarseqConfiguration*)configuration)->tabconflicts[j][configuration->config[j]])
	{configuration->var_conflict_size++;
         for (int k=0; k < ((CarseqConfiguration*)configuration)->tabconflicts[j][configuration->config[j]]; k++)
	   configuration->var_conflict.push_back(j);}
      }
}



/* taille max possible pour un voisinage : tous les échanges avec les variables en conflit */

void Carseq::adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors)
  { int var_conflict_size = ((CarseqConfiguration*)configuration)->var_conflict_size;
    if 	(maxneighbors > var_conflict_size * (nbvar -1))
      maxneighbors = var_conflict_size * (nbvar -1);
    if 	(minneighbors > var_conflict_size * (nbvar -1))
      minneighbors = var_conflict_size * (nbvar -1);
  }


int Carseq::tabuindex(Move* move,Configuration* configuration)
{return (((SwapMove*)move)->variable1 * domainsize + configuration->config[((SwapMove*)move)->variable2]);
}

int Carseq::tabuinverseindex(Move* move,Configuration* configuration)
{return (((SwapMove*)move)->variable1 * domainsize + configuration->config[((SwapMove*)move)->variable1]);}


void Carseq::best_config_analysis()
  {;}
//  {best_config_write();}




