/* Tout ce qui concerne le celar  ...  (sauf les spécifités pour l'incrémentalité qui se trouvent
dans celarincr.cc et celarfullincr.cc) */


#include <cerrno>
#include <stdio.h>


#include <list>
#include <vector>
#include <string>
#include <set>
using namespace std;
#include <fstream>
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "celar.h"
#include "autotuning2.h"

/* taille du plus grand domaine utilisé dans celar6,7,8 = 44 */


extern ofstream* ofile;  // le fichier de sortie
extern Stat_GWW * Statistiques; 


CelarCSProblem* celar_problem_creation (ifstream & file, int varcost)
{ int nbvar; int nbconst;
  lire_debut_celar (file,nbvar,nbconst);
  int **constraint1; 

  constraint1 =  csp_constraintdatastructure(nbvar);

  int* dom = new int[nbvar];
  
  vector<int>* tabdom = new vector<int> [nbvar] ; // les differents types de domaines 
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar];

  int*  numerovariable = new int[nbvar];
  int* refvalue = new int[nbvar];
  int* refcost = new int[nbvar];
  int* valconst= new int[nbconst+1]; //le numero de contraintes commence a 1, il faut dimensionner a nbconst+1
  int*  distance= new int[nbconst+1];
  lire_suite_celar(file,nbvar,dom,numerovariable,varcost,refvalue,refcost);
  lire_fichier_celar(file,nbvar,connex,constraint1,
		     distance,valconst,numerovariable);
  CelarCSProblem*    problem = probleme_celar
    (nbvar,nbconst,tabdom,dom,constraint1,distance,valconst,numerovariable,varcost,refvalue,refcost);
// mise en place des domaines et connexions
  problem->set_domains_connections(dom,tabdom,connex);
  return problem;
}



/* creation du probleme (appel au constructeur) */
CelarCSProblem* probleme_celar(int nbvar,int nbconst,vector<int>* tabdomaines,
			       int * domaines,int **constraint1,int *distance,
			       int * valconst,int *numerovariable , int varcost, int* refvalue, int* refcost)
{CelarCSProblem* p1 = new CelarCSProblem (nbvar,nbconst);
 p1->inittables(tabdomaines);
 p1->neighborhoodlength=0;
 for (int i=0;i<nbvar;i=i+2)
   {
     p1->neighborhoodlength += tabdomaines[domaines[i]].size()-1;
   }
 p1->constraints=constraint1;
 p1->valconst=valconst;
 p1->distance=distance;
 p1->numerovariable=numerovariable;
 p1->refvalue=refvalue;
 p1->refcost = refcost;
 p1->varcost=varcost;
 p1->domainsize=44;
 for(int i =0; i<8; i++)
   {for (int k=0; k<(int)tabdomaines[i].size() ;k++)
     for (int k1=0; k1 < (int)tabdomaines[i].size(); k1++)
       if (p1->tabsymdomaines[tabdomaines[i][k]]== tabdomaines[i][k1])
	 p1->tabsymindex[i].push_back(k1);
   }
 return p1;

}


void CelarCSProblem::inittables(vector<int>* tabledomaines)
{
for (int i=0;i<8;i++)
    { tabindex[i]=new int[800];
 for (int j=1;j<800;j++)
   tabindex[i][j]=-1;
    }
for (int i=0;i<8;i++)

tailledomaines[0]=48;
tailledomaines[1]=44;
tailledomaines[2]=22;
tailledomaines[3]=36;
tailledomaines[4]=24;
tailledomaines[5]=6;
tailledomaines[6]=42;
tailledomaines[7]=22;
    int  tabledomaines0 []= { 16,  30,  44,  58,  72,  86, 100, 114, 128, 142, 156, 170, 240, 254, 268, 282, 296, 310, 324, 338, 352, 366, 380, 394, 408, 414, 428, 442, 456, 470, 478, 484, 498, 512, 526, 540, 554, 652, 666, 680, 694, 708, 722, 736, 750, 764, 778, 792};
    int tabledomaines1[]= { 16,  30,  44,  58,  72,  86, 100, 114, 128, 142, 156, 254, 268, 282, 296, 310, 324, 338, 352, 366, 380, 394, 414, 428, 442, 456, 470, 484, 498, 512, 526, 540, 554, 652, 666, 680, 694, 708, 722, 736, 750, 764, 778, 792};
    int  tabledomaines2[]= {30,  58,  86, 114, 142, 268, 296, 324, 352, 380, 414, 442, 470, 498, 526,554, 652, 680, 708, 736, 764,792};
    int  tabledomaines3 []= {30,  44,  58,  72,  86, 100 ,114, 128, 142, 268, 282, 296, 310, 324, 338, 352, 366, 380, 428, 442, 456, 470, 484, 498, 512, 526, 540, 666, 680, 694, 708, 722, 736, 750, 764, 778};
    int  tabledomaines4 []= { 16,  30,  58,  86, 114, 142, 254, 268, 296, 324, 352, 380, 414, 442, 470, 498, 526, 554, 652, 680, 708, 736, 764, 792};
    int  tabledomaines5 []= {142, 170, 240, 380, 408, 478};
    int  tabledomaines6 []= { 30,  44,  58,  72,  86, 100, 114, 128 ,142, 156, 268, 282, 296, 310, 324, 338, 352, 366, 380, 394, 414, 428, 442, 456, 470, 484, 498, 512, 526, 540, 554, 652, 666, 680, 694, 708, 722, 736, 750, 764, 778, 792};
    int  tabledomaines7 []= {16,  30,  44,  58,  72,  86, 100, 114, 128,  142, 156, 254, 268, 282, 296, 310, 324, 338, 352, 366, 380, 394};
    for (int i =0 ; i<tailledomaines[0];i++)
      {	tabledomaines[0].push_back(tabledomaines0[i]);
      tabindex[0][tabledomaines0[i]]=i;
      }
    //    int k=0;
    
    for (int i =0 ; i<tailledomaines[1];i++)
	tabledomaines[1].push_back(tabledomaines1[i]);
    for (int i =0 ; i<tailledomaines[1]-1;i++)
	
	for (int k= tabledomaines1[i]; k< tabledomaines1[i+1]; k++)
	  tabindex[1][k]=i;
    for (int k= tabledomaines1[tailledomaines[1]-1]; k<800; k++)
      tabindex[1][k]= tailledomaines[1]-1;


    for (int i =0 ; i<tailledomaines[2];i++)
	tabledomaines[2].push_back(tabledomaines2[i]);
    for (int i =0 ; i<tailledomaines[2]-1;i++)
	
	for (int k= tabledomaines2[i]; k< tabledomaines2[i+1]; k++)
	  tabindex[2][k]=i;
    for (int k= tabledomaines2[tailledomaines[2]-1]; k<800; k++)
      tabindex[2][k]= tailledomaines[2]-1;

    for (int i =0 ; i<tailledomaines[3];i++)
	tabledomaines[3].push_back(tabledomaines3[i]);
    for (int i =0 ; i<tailledomaines[3]-1;i++)
	for (int k= tabledomaines3[i]; k< tabledomaines3[i+1]; k++)
	  tabindex[3][k]=i;
    for (int k= tabledomaines3[tailledomaines[3]-1]; k<800; k++)
      tabindex[3][k]= tailledomaines[3]-1;

    for (int i =0 ; i<tailledomaines[4];i++)
	tabledomaines[4].push_back(tabledomaines4[i]);
    for (int i =0 ; i<tailledomaines[4]-1;i++)
	for (int k= tabledomaines4[i]; k< tabledomaines4[i+1]; k++)
	  tabindex[4][k]=i;
    for (int k= tabledomaines4[tailledomaines[4]-1]; k<800; k++)
      tabindex[4][k]= tailledomaines[4]-1;

    for (int i =0 ; i<tailledomaines[5];i++)
	tabledomaines[5].push_back(tabledomaines5[i]);
    for (int i =0 ; i<tailledomaines[5]-1;i++)
      for (int k= tabledomaines5[i]; k< tabledomaines5[i+1]; k++)
	  tabindex[5][k]=i;
    for (int k= tabledomaines5[tailledomaines[5]-1]; k<800; k++)
      tabindex[5][k]= tailledomaines[5]-1;

    for (int i =0 ; i<tailledomaines[6];i++)
	tabledomaines[6].push_back(tabledomaines6[i]);
    for (int i =0 ; i<tailledomaines[6]-1;i++)
      for (int k= tabledomaines6[i]; k< tabledomaines6[i+1]; k++)
	  tabindex[6][k]=i;
    for (int k= tabledomaines6[tailledomaines[6]-1]; k<800; k++)
      tabindex[6][k]= tailledomaines[6]-1;

    for (int i =0 ; i<tailledomaines[7];i++)
	tabledomaines[7].push_back(tabledomaines7[i]);
    for (int i =0 ; i<tailledomaines[7]-1;i++)
      for (int k= tabledomaines7[i]; k< tabledomaines7[i+1]; k++)
	  tabindex[7][k]=i;
    for (int k= tabledomaines7[tailledomaines[7]-1]; k<800; k++)
      tabindex[7][k]= tailledomaines[7]-1;


    /* la valeur de la variable impaire en fonction de la paire */
for(int i =0; i< 800; i++)
  tabsymdomaines[i]=0;
tabsymdomaines[16]=254;
tabsymdomaines[30]=268;
tabsymdomaines[44]=282;
tabsymdomaines[58]=296;
tabsymdomaines[72]=310;
tabsymdomaines[86]=324;
tabsymdomaines[100]=338;
tabsymdomaines[114]=352;
tabsymdomaines[128]=366;
tabsymdomaines[142]=380;
tabsymdomaines[156]=394;
tabsymdomaines[170]=408;
tabsymdomaines[240]=478;
tabsymdomaines[254]=16;
tabsymdomaines[268]=30;
tabsymdomaines[282]=44;
tabsymdomaines[296]=58;
tabsymdomaines[310]=72;
tabsymdomaines[324]=86;
tabsymdomaines[338]=100;
tabsymdomaines[352]=114;
tabsymdomaines[366]=128;
tabsymdomaines[380]=142;
tabsymdomaines[394]=156;
tabsymdomaines[408]=170;
tabsymdomaines[414]=652;
tabsymdomaines[428]=666;
tabsymdomaines[442]=680;
tabsymdomaines[456]=694;
tabsymdomaines[470]=708;
tabsymdomaines[478]=240;
tabsymdomaines[484]=722;
tabsymdomaines[498]=736;
tabsymdomaines[512]=750;
tabsymdomaines[526]=764;
tabsymdomaines[540]=778;
tabsymdomaines[554]=792;
tabsymdomaines[652]=414;
tabsymdomaines[666]=428;
tabsymdomaines[680]=442;
tabsymdomaines[694]=456;
tabsymdomaines[708]=470;
tabsymdomaines[722]=484;
tabsymdomaines[736]=498;
tabsymdomaines[750]=512;
tabsymdomaines[764]=526;
tabsymdomaines[778]=540;
tabsymdomaines[792]=554;

 
}




CelarCSProblem::CelarCSProblem (int nvar, int nconst) :
BinaryCSProblem (nvar,nconst,0){domainsize=40;}




/* pour le celar, on affecte 2 variables consecutives */
void CelarCSProblem::random_configuration(Configuration* configuration)
{ for (int j=0;j<nbvar;j=j+2)
  {if (varcost && refvalue[j]!=0 && refcost[j]==0)  // variable préinstanciée
    {configuration->config[j]=refvalue[j];
     configuration->config[j+1]=refvalue[j+1];
    }
  else
    {  int indice = (int) (drand48() * variable_domainsize(j));
    configuration->config[j]= index2value(indice,j);
    configuration->config[j+1] = tabsymdomaines[index2value(indice,j)];
    }
  }
}
 
/* pour le Celar, on modifie explicitement la variable paire (la variable impaire suivra automatiquement) */
int CelarCSProblem::random_variable(Configuration* configuration)
{int var_changee= (int) (drand48() * nbvar);
 if ((var_changee %2) == 1) var_changee--;
 return var_changee;}


int CelarCSProblem::random_conflict_variable(Configuration* configuration)
{int var_changee= CSProblem::random_conflict_variable(configuration);
 if ((var_changee %2) == 1) var_changee--;
 return var_changee;}


/* passage de l'index dans le domaine à la valeur */
int CelarCSProblem::index2value (int index, int var)
{return tabdomains[domains[var]][index];   
}

/* passage d'une valeur à l'index dans le domaine*/
int CelarCSProblem::value2index(int value, int var)
{ return tabindex[domains[var]][value];
}


void CelarCSProblem::incr_update_conflicts(IncrCSPConfiguration* configuration, Move* move)

{ int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  incr_update_conflicts_1(configuration,var_changee,val_changee);
  incr_update_conflicts_1(configuration,var_changee+1,tabsymdomaines[val_changee]);
}


void CelarCSProblem::fullincr_update_conflicts(FullincrCSPConfiguration* configuration, Move* move)

{ int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  fullincr_update_conflicts_1(configuration,var_changee,val_changee);
  fullincr_update_conflicts_1(configuration,var_changee+1,tabsymdomaines[val_changee]);
}


void CelarCSProblem::fullincr_update_conflicts_1(FullincrCSPConfiguration* configuration,int var_changee, int val_changee)
{  
  int valmin; int valmax;
  int numero_contrainte; int dist;
  for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
    {   
      numero_contrainte =  constraints[*vi][var_changee];
      dist= distance[numero_contrainte];
      valmax = val_changee + dist;
      valmin = val_changee - dist -1;
      if (valmin < tabdomains[domains[*vi]][0])
	valmin= tabdomains[domains[*vi]][0] -1 ;
      if (valmax > tabdomains[domains[*vi]][tabdomains[domains[*vi]].size()-1])
	  valmax= tabdomains[domains[*vi]][tabdomains[domains[*vi]].size()-1];
      for (int k = value2index (valmin,*vi)+1 ; k < value2index(valmax,*vi) +1; k++)
	configuration->tabconflicts[*vi][k]  +=valconst[numero_contrainte];
      valmax= configuration->config[var_changee] + dist;
      valmin= configuration->config[var_changee] - dist -1;
      if (valmin < tabdomains[domains[*vi]][0])
	valmin= tabdomains[domains[*vi]][0] -1;
      if (valmax > tabdomains[domains[*vi]][tabdomains[domains[*vi]].size()-1])
	valmax= tabdomains[domains[*vi]][tabdomains[domains[*vi]].size()-1];
      for (int k = value2index (valmin,*vi)+1 ; k < value2index(valmax,*vi) +1; k++)
	configuration->tabconflicts[*vi][k]  -=valconst[numero_contrainte];
    }
}

      


void CelarCSProblem::incr_update_conflicts_1(IncrCSPConfiguration* configuration,int var_changee, int val_changee)
{  
  int nb_conflits_apres=0;
  int numero_contrainte;
  for (vector<int>::iterator vi= connections[var_changee].begin();vi!= connections[var_changee].end(); vi++)
    {   
      numero_contrainte =  constraints[*vi][var_changee];
    if (abs (configuration->config[*vi]   - configuration->config[var_changee])
	<= distance[numero_contrainte]) 

      configuration->tabconflicts[*vi]-=valconst[numero_contrainte];
    if (abs (configuration->config[*vi] - val_changee)
	<= distance[numero_contrainte]) 
      {nb_conflits_apres += valconst[numero_contrainte];

	  configuration->tabconflicts[*vi]+=valconst[numero_contrainte];
      }
    }
  // cable pour configuration   incr   (set_conflicts conditionnel ...)
  //  configuration->set_variableconflicts(var_changee,nb_conflits_apres);
   configuration->tabconflicts[var_changee]=nb_conflits_apres;
   if (varcost && refcost[var_changee]!=0 && refvalue[var_changee] != val_changee)
     configuration->tabconflicts[var_changee]+= refcost[var_changee];
     
}




void CelarCSProblem::move_execution(Configuration* configuration,Move* move)
{ 
  OpProblem::move_execution(configuration,move);
  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  configuration->config[var_changee]=val_changee;
  configuration->config[var_changee+1]= tabsymdomaines[val_changee];
}

int CelarCSProblem::move_evaluation(Configuration* configuration,Move* move)
{ 
  int var_changee = ((CSPMove*)move)-> variable;
  int val_changee = ((CSPMove*)move)-> value;
  int  valeur_config_changee = 
    move_evaluation1(configuration,configuration->valuation,var_changee,val_changee);
  valeur_config_changee = 
    move_evaluation1 (configuration, valeur_config_changee,var_changee+1,
		      tabsymdomaines[val_changee]);

  //  *ofile << "  mouvement " << var_changee << " "  << val_changee << " valeur " << valeur_config_changee << endl;
  return valeur_config_changee;
}
	 
/* evaluation d'un mouvement elementaire : redefini de manière optimisée dans
celarincr.cc et celarfullincr.cc 
 -  en commentaire : version non optimisée pour configuration  quelconque  */

/*
int CelarCSProblem::move_evaluation1(Configuration* configuration,int valeur,int var_changee,int val_changee)

{
  return(valeur+configuration->get_conflicts_problem(this,var_changee,val_changee)
               -configuration->get_conflicts_problem(this,var_changee,configuration->config[var_changee]));
}
*/


void CelarCSProblem::best_config_write()
{*ofile<< " meilleure solution " << endl;
for (int i = 0; i< nbvar ; i++)
  *ofile << " variable " << numerovariable[i] << " : " << best_config->config[i] << endl;
}


int CelarCSProblem::compute_conflict(Configuration* configuration, int var , int val)
{
  int nb_conflits =0;
  int numero_contrainte;
  if (varcost)
    {if (refvalue[var]!=0)
      if (refvalue[var] != val) 
	nb_conflits+= refcost[var];
    }
      
  for (vector<int>::iterator vi= connections[var].begin();vi!= connections[var].end(); vi++)
    { 
	numero_contrainte =  constraints[*vi][var];
      if (abs (configuration->config[*vi] - val)
	  <= distance[numero_contrainte]) 
	nb_conflits += valconst[numero_contrainte];
    }
  return nb_conflits;
}

int CelarCSProblem::tabuindex(Move* move, Configuration* configuration)
{return (((CSPMove*)move)->variable * domainsize + value2index(((CSPMove*)move)->value,((CSPMove*)move)->variable));}

int CelarCSProblem::tabuinverseindex(Move* move, Configuration* configuration)
{return (((CSPMove*)move)->variable * domainsize +  value2index(configuration->config[((CSPMove*)move)->variable],
								((CSPMove*)move)->variable));}


// 1ere ligne du fichier : nb variables, nb contraintes
void lire_debut_celar(ifstream & file, int & nbvar, int & nbconst)
{    file >> nbvar ; file >> nbconst;
 }



void lire_suite_celar(ifstream & file,int nbvar,int* domaines,int * numerovariable, int varcost, int* refvalue, int* refcost)
{
  int i1,i2,i3,i4;
  for(int i=0;i<nbvar;i++)
    {	
    file >> i1 ; file >> i2;
    
	//	char str[10];
	//	sprintf(str,"%d\0",i1);
    domaines[i]=i2;
    numerovariable[i]=i1;
    if (varcost)
      {file >> i3; 
      refvalue[i]=i3;
      if (i3) 
	{file >> i4;
	refcost[i]=i4;}
      else refcost[i]=0;
      //	  cout << i1 << " " << i2 << " " << i3 << " " << i4 << endl;
      }
    }
  if (varcost)
    { file >> i1; file >> i2 ; file >> i3; file >> i4;
    for (int i =0; i< nbvar; i++)
      {if (refvalue[i]!=0)
	if (refcost[i] == 1) refcost[i]=i4;
	else if (refcost[i]==2) refcost[i]=i3;
	else if (refcost[i]==3) refcost[i]=i2;
	else if (refcost[i]==4) refcost[i]=i1;
      }
    }
  
}


void lire_fichier_celar (ifstream& file, int nbvar, vector<int>* connexions,int ** constraint1, int* distance,
int* valconst,
int* numerovariable)
{ int i1,i2,i3,i4,i5;
int k1; int k2; int k3; 

 int  j=1; // on commence a numeroter a 1 (constraint1[i][j]=0 signifie pas de contraintes)
int c1,c2,c3,c4;
 file >> c1; file >> c2; file >> c3; file >> c4;
      while (!file.eof())
	{	file >> i1 ; file >> i2; file >> i3 ; file >> i4; file >> i5;
	for (int i=0; i<nbvar; i++)
	  {k3=  numerovariable[i];
	  if (k3==i1) k1=i;
	  if (k3==i2) k2=i;
	  }
	connexions[k1].push_back(k2);
	connexions[k2].push_back(k1);
	if (i3 !=0)
	  {constraint1[k1][k2]=j;
           constraint1[k2][k1]=j;
	  distance[j]=i4;
	  if (i5==0) valconst[j]=0;
	  else if (i5==1) valconst[j]=c4;
	  else if (i5==2) valconst[j]=c3;
	  else if (i5==3) valconst[j]=c2;
	  else if (i5==4) valconst[j]=c1;
	  j++;
	  }
	}
}


int celarcsp(int argc, char** argv, int tuningmode) {

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
  int varcost=0;
  if  ((string)argv[2]== "celar9.txt" || (string)argv[2]=="celar10.txt")
    varcost=1;

  // lecture des paramètres de l'algo et création de l'objet algo
  IncompleteAlgorithm* algo = algo_creation (argv, narg, taille, graine1, nbessais);

  // allocation de l'objet pour les stats
  Statistiques=new Stat_GWW (1, nbessais);
  
  // argument pour la trace
  arguments_tracemode(argv,narg);
  // pour la recuperation du signal 10
  sigaction();
  // argument de temps maximum 
  double maxtime;
  if (tuningmode)  arguments_tempscpu (argv,narg,maxtime);

  // creation du probleme (lecture des données, creations des structures de données)
  CelarCSProblem* problem = celar_problem_creation (file,varcost);
  // les bornes inférieures connues
  if ((string)argv[2]=="celar9.txt") problem->lower_bound = 15571;
  if ((string)argv[2]=="celar10.txt") problem->lower_bound = 31516;
  if ((string)argv[2]=="celar6.txt") problem->lower_bound = 3389;
  if ((string)argv[2]=="celar8.txt") problem->lower_bound = 150;
  if ((string)argv[2]=="celar7.txt") problem->lower_bound = 300000;
  // creation de la population et initialisation 
  // La population : tableau de configurations
  Configuration* population[taille];
  problem->init_population(population,taille);
  problem->allocate_moves();

  // initialisation des statistiques
  Statistiques->init_pb(0);

  if (tuningmode) 

    autosolving((LSAlgorithm*)algo,population,problem,0,graine1,nbessais,maxtime,1000000);
  else 
  // boucle sur les essais 

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



