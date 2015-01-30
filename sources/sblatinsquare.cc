#include <list>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <algorithm>
using namespace std;
#include <fstream>
#include "incop.h"
#include "csproblem.h"
#include "latinsquare.h"
#include "move.h"

// le problème du carré latin équilibré (cf article Gomes  CPAIOR 2004)

extern ofstream* ofile;  // le fichier de sortie

SBLatinsquare* probleme_carrelatin_equilibre (int nbvar)
{SBLatinsquare* p1 = new SBLatinsquare (nbvar);
return p1;
}

SBLatinsquare* sblatinsquare_problem_creation (int nbvar)
{
  int* dom = new int[nbvar*nbvar];

  vector<int>* tabdom = new vector<int> [nbvar*nbvar] ; // les differents types de domaines
  // Initialisation des structures de données des problèmes
  vector<int>* connex = new vector<int> [nbvar*nbvar];

  SBLatinsquare * problem = probleme_carrelatin_equilibre(nbvar);

  problem->set_domains_connections(dom,tabdom,connex);
  problem->init_domain_tabdomain();
  return problem;


 return problem;
}
SBLatinsquare::SBLatinsquare (int nvar) : Latinsquare (nvar){;}




int compute_distance_line(Configuration* configuration, int l, int i, int j, int squaresize)
{int placei=0; int placej=0;
 for (int k=0; k< squaresize; k++)
   {if (configuration->config[squaresize*l+k]== i) placei=k;
   if (configuration->config[squaresize*l+k]== j) placej=k;
   if (placei && placej) break;}
 return (abs(placei-placej));
   }

int distance_balance (int sumdist,int  size)
{// return abs (3* sumdist - size * (size+1))
  return abs (sumdist - (size * (size+1)/3))  // donne un poids moins important aux contraintes d'équilibrage
   ;}

int distance_balance1 (int sumdist,int  obj)
{
  return abs (sumdist - obj)
   ;}



// evaluation et remplissage de la structure incrementale : le nombre d'occurrences de chaque valeur j dans une colonne i  + somme des distances pour contraintes d'équilibrage
int SBLatinsquare::config_evaluation (Configuration* configuration)
{

 int valeur= 3* Latinsquare::config_evaluation(configuration);
  //    int valeur= 2* Latinsquare::config_evaluation(configuration);
 // int valeur = Latinsquare::config_evaluation(configuration);
 int valcl = valeur;

 *ofile << " contraintes de carré latin " << valcl;
 //   pour l'equilibre  moyenne distances (i,j) = n+1/3
 for (int i=0; i< squaresize-1; i++)
   for (int j=i+1 ; j< squaresize; j++)
     {int sumdist=0;
     for (int k=0; k< squaresize; k++)
       {
       sumdist+=compute_distance_line(configuration,k,i,j,squaresize);
       }
     configuration->incr_conflicts(squaresize+ i*squaresize + j,0,0,sumdist);
     //     *ofile << " i " << i << " j " << j << " somme " << sumdist << endl;
     valeur+= distance_balance (sumdist ,squaresize);
     }
 *ofile << " contraintes d'équilibrage " << valeur-valcl << endl;
 return valeur;
}





// evaluation d'un mvt
// 2 parties : carre latin  et equilibrage
// la structure des donnees des conflits est en 2 parties
// tabconflicts [i,j]  colonne i valeur j  : nombre d'occurrences de la valeur j dans la colonne i
// tabconflicts [squaresize + squaresize * valeur (ligne,col) + valeur(ligne,col2), 0] :  avec valeur(ligne,col) < valeur(ligne,col2) : somme des distances entre les deux valeurs

int SBLatinsquare::move_evaluation (Configuration* configuration,Move* move)
{
  int line = ((ColSwMove*)move)->line;
  int variable1 = squaresize*line + ((ColSwMove*)move)-> variable1;
  int variable2 = squaresize*line + ((ColSwMove*)move)-> variable2;
  int val1= configuration->config[variable1];
  int val2= configuration->config[variable2];
  int objdist= squaresize * (squaresize+1)/3;
  /* valeur sans l'increment des contraintes d'équilibrage */
   int nval = 3* Latinsquare::move_evaluation(configuration,move) - 2*configuration->valuation;
  //    int nval = 2* Latinsquare::move_evaluation(configuration,move) - configuration->valuation;
   //  int nval =  Latinsquare::move_evaluation(configuration,move);

  int var1,var2;
  if (variable1< variable2)
    {var1=((ColSwMove*)move)-> variable1 ;var2=((ColSwMove*)move)-> variable2;}
  else
    {var1=((ColSwMove*)move)-> variable2;var2=((ColSwMove*)move)-> variable1;
    val1=configuration->config[variable2]; val2 = configuration->config[variable1]; }
  int deltadist=0; int ancdist;
  for (int i=0;i<var1;i++)
      {  int val= configuration->config[squaresize*line + i];
        deltadist = var2-var1;
        //cout << "cas A :  Val " << val << " val1 " << val1 << " val2 " << val2 << endl;
        if (val < val1)
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val*squaresize + val1][0] ;
	else
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val1*squaresize + val][0] ;
        nval += distance_balance1 (ancdist+ deltadist, objdist) -
	  distance_balance1 (ancdist, objdist);
        if (val < val2)
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val*squaresize + val2][0]  ;
	else
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val2*squaresize + val][0]  ;
        nval += distance_balance1 (ancdist - deltadist, objdist) -
	  distance_balance1 (ancdist, objdist);
      }
  for (int i=var1+1;i<var2;i++)
      {  int val= configuration->config[squaresize*line + i];
      //        cout << "cas B :  Val " << val << " val1 " << val1 << " val2 " << val2 << endl;
      deltadist = var2+ var1 -2 *i ;
      if (val < val1)
	ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val*squaresize + val1][0]  ;
      else
	ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val1*squaresize + val][0]  ;
      nval += distance_balance1 (ancdist+ deltadist, objdist) -
	distance_balance1 (ancdist, objdist);
      if (val < val2)
	ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val*squaresize + val2][0]  ;
      else
	ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val2*squaresize + val][0]  ;
      nval += distance_balance1 (ancdist - deltadist, objdist) -
	distance_balance1 (ancdist, objdist);

      }
  for (int i=var2+1;i<squaresize;i++)
      {  int val= configuration->config[squaresize*line + i];
        deltadist = var2 - var1;
	//        cout << "cas C :  Val " << val << " val1 " << val1 << " val2 " << val2 << endl;
	if (val < val2)

	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val*squaresize + val2][0]  ;
	else
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val2*squaresize + val][0]  ;
	nval += distance_balance1 (ancdist+ deltadist, objdist) -
	  distance_balance1 (ancdist, objdist);
	if (val < val1)
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val*squaresize + val1][0] ;
	else
	  ancdist = ((FullincrCSPConfiguration*)configuration)->tabconflicts[squaresize+ val1*squaresize + val][0]  ;
	nval += distance_balance1 (ancdist - deltadist, objdist) -
	  distance_balance1 (ancdist, objdist);
      }
  return (nval);
}


void SBLatinsquare::fullincr_update_conflicts (FullincrCSPConfiguration* configuration,Move* move)
{
    int line = ((ColSwMove*)move)->line;
    //*ofile << "FINCR " << line << " " << ((ColSwMove*)move)-> variable1 << " " << ((ColSwMove*)move)-> variable2 << endl;
  int variable1 = squaresize*line + ((ColSwMove*)move)-> variable1;
  int variable2 = squaresize*line + ((ColSwMove*)move)-> variable2;
  int val1= configuration->config[variable1];
  int val2= configuration->config[variable2];
  configuration->tabconflicts[ ((ColSwMove*)move)-> variable1][val1]--;
  configuration->tabconflicts[ ((ColSwMove*)move)-> variable1][val2]++;
  configuration->tabconflicts[ ((ColSwMove*)move)-> variable2][val2]--;
  configuration->tabconflicts[ ((ColSwMove*)move)-> variable2] [val1]++;

int var1,var2;
if (variable1< variable2)
  {var1=((ColSwMove*)move)-> variable1 ;var2=((ColSwMove*)move)-> variable2;}

  else
    {var1=((ColSwMove*)move)-> variable2;var2=((ColSwMove*)move)-> variable1;
    val1= configuration->config[variable2];val2= configuration->config[variable1]; }
// le stockage des contraintes d'equilibrage : on stocke la somme des differences entre les places de val1 et val2
  int deltadist=0;
  // les distances avec valeurs val placées avant val1
  for (int i=0;i<var1;i++)
      {  int val= configuration->config[squaresize*line + i];
        deltadist = var2-var1;
        if (val < val1)
	  {
           configuration->tabconflicts[squaresize+ val*squaresize + val1][0]+=deltadist;}
	else
	  {
            configuration->tabconflicts[squaresize+ val1*squaresize + val][0]+= deltadist  ;}
        if (val < val2)
	  {
           configuration->tabconflicts[squaresize+ val*squaresize + val2][0] -= deltadist ;}
	else
	  {
            configuration->tabconflicts[squaresize+ val2*squaresize + val][0] -=deltadist  ;}
      }
  // les distances avec les valeurs val placées entre val1 et val2
  for (int i=var1+1;i<var2;i++)
      {  int val= configuration->config[squaresize*line + i];
      deltadist = var2+ var1 -2 *i ;
      if (val < val1)
	{
	configuration->tabconflicts[squaresize+ val*squaresize + val1][0]+= deltadist  ;}
      else
	{
	configuration->tabconflicts[squaresize+ val1*squaresize + val][0]+= deltadist  ;}
      if (val < val2)
	{
	configuration->tabconflicts[squaresize+ val*squaresize + val2][0] -= deltadist  ;}
      else
	{
	configuration->tabconflicts[squaresize+ val2*squaresize + val][0] -=deltadist  ;}
      }
  // les distances avec les valeurs val placées après val2
  for (int i=var2+1;i<squaresize;i++)
      {  int val= configuration->config[squaresize*line + i];
        deltadist = var2 - var1;
	if (val < val2)
	  {
	  configuration->tabconflicts[squaresize+ val*squaresize + val2][0] += deltadist  ;}
	else
	  {
	  configuration->tabconflicts[squaresize+ val2*squaresize + val][0] += deltadist  ;}
	if (val < val1)
	  {
	  configuration->tabconflicts[squaresize+ val*squaresize + val1][0] -= deltadist  ;}
	else
	  {
	  configuration->tabconflicts[squaresize+ val1*squaresize + val][0] -= deltadist  ;}
      }

  }









