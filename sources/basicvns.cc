/* le fichier comportant les méthodes des différents algorithmes (gww
   et recherche locale) , des métaheuristiques, des classes de
   configurations, */


#include <list>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <stack>
#include <random>

using namespace std;
#include <fstream>
#include <math.h>
#include <unistd.h>
#include <iostream>

#include "timer.h"
#include "basicvns.h"
#include "incoputil.h"
#include "csproblem.h"


// les variables globales
extern Stat_GWW * Statistiques; // defini dans incoputil
extern int TRACEMODE;    // indicateur de trace
extern ofstream* ofile;  // le fichier de sortie
 
/* --- BVNS --- */

BVNSAlgorithm::BVNSAlgorithm (int kmax, int maxTime) : kmax(kmax),maxTime(maxTime) 
{ 
	movements.push_back(new PFlip(5)); // 
	movements.push_back(new Swap()); // 
	movements.push_back(new TwoExchange()); //  
	movements.push_back(new KempeChain()); //  
};
void BVNSAlgorithm::BVNSAlgorithm::run (OpProblem *problem, Configuration** s)
{
	time(&startTime);
	walkalgo->nbhsearch->initsearch();
	problem->compute_var_conflict(*s);
	previous = problem->create_configuration();
	previous->copy_element(*s);
	problem->compute_var_conflict(previous);
	do
	{
		int i=0;
		do
		{
			movements[i]->shake(problem,(*s));
			Statistiques->nb_moves[Statistiques->current_try]++;
			problem->compute_var_conflict(*s);
			walkalgo->randomwalk(problem,(*s));
			problem->compute_var_conflict(*s);
			if (previous->var_conflict.size() < (*s)->var_conflict.size())
			{
				(*s)->copy_element(previous);
				i = 0;
			}
			else
			{
				previous->copy_element((*s));
				i++;
			}
			problem->compute_var_conflict(*s);
			problem->compute_var_conflict(previous);
//cout << "\n --------- Fin Boucle interieur --------- " << endl;
//			cout << "Nombre de conflit restant = " << (*s)->var_conflict.size() << std::endl;
//			cout << "Mouvement : i / kmax = " << i << "/" << kmax << std::endl;
//			cout << "Time Diff(currTime, startTime) / maxTime = " << difftime(time(&currTime),startTime) << " / " << maxTime << std::endl;
		} while (difftime(time(NULL),startTime) < maxTime && i < kmax && (*s)->var_conflict.size()!=0);
	} while ( difftime(time(NULL),startTime) < maxTime && (*s)->var_conflict.size()!=0);
//	if (previous->valuation > (*s)->valuation);
	if (previous->var_conflict.size() < ((*s))->var_conflict.size())
	{
		((*s))->copy_element(previous);
	}
	problem->compute_var_conflict(*s);
	cout << "Conflit restant " <<  (*s)->var_conflict.size() << endl;
}

/* --- ---- --- */

/* --- NeighborStruc --- */

Configuration* PFlip::shake(OpProblem* problem, Configuration* s)
{
	if (s->var_conflict.size() <= 0)
		return NULL;
	int i;
	for (i=0; i<p && s->var_conflict.size() >0; i++)
	{
		
		CSPMove* m = (CSPMove*)problem->create_move();
//		m->variable = ((CSProblem*)problem)->random_variable(s);
		m->variable = ((CSProblem*)problem)->random_conflict_variable(s);

		m->value = ((CSProblem*)problem)->random_value(m->variable,s->config[m->variable]);
		m->valuation = ((CSProblem*)problem)->move_evaluation(s,m);
		problem->move_execution(s,m);
		s->valuation = problem->config_evaluation(s);
	}
	return s;
}

Configuration* TwoExchange::shake(OpProblem* problem, Configuration *s)
{
	if (s->var_conflict.size() <= 0)
		return NULL;
	CSProblem* p = (CSProblem*)problem;
	int var1 = p->random_conflict_variable(s);
	int var2 = p->random_variable(s);
	// test if var1 = var2, only allow a few test;
	int i = 0;
	while (var2 == var1 && i < 5)
	{
		p->random_variable(s);
		i++;
	}

	CSPMove* m1 = (CSPMove*)problem->create_move();
	CSPMove* m2 = (CSPMove*)problem->create_move();
	m1->variable = var1;
	m2->variable = var2;
	m1->value = s->config[var2];
	m2->value = s->config[var1];
	m1->valuation = p->move_evaluation(s,m1);
	problem->move_execution(s,m1);
	m2->valuation = p->move_evaluation(s,m2);
	problem->move_execution(s,m2);
	s->valuation = problem->config_evaluation(s);
	return s;
}

Configuration* Swap::shake(OpProblem* problem, Configuration *s)
{
	if (s->var_conflict.size() <= 0)
		return NULL;
	CSProblem* p = (CSProblem*) problem;
	int var1 = p->random_conflict_variable(s);
	int var2 = (var1 == s->nbvar-1) ? var1-1 : var1+1;

	CSPMove* m1 = (CSPMove*) problem->create_move();
	CSPMove* m2 = (CSPMove*) problem->create_move();
	m1->variable = var1;
	m2->variable = var2;
	m1->value = s->config[var2];
	m2->value = s->config[var1];
	m1->valuation = p->move_evaluation(s,m1);
	problem->move_execution(s,m1);
	m2->valuation = p->move_evaluation(s,m2);
	problem->move_execution(s,m2);
	s->valuation = problem->config_evaluation(s);
	return s;
}

Configuration* KempeChain::shake(OpProblem* problem, Configuration *s)
{
	if (s->var_conflict.size() <= 0)
		return NULL;
	CSProblem* p = dynamic_cast<CSProblem*>(problem);
	int varStart = p->random_variable(s);
	int i = 0;
	while (p->connections[varStart].size() == 0)
	{
		if (i == 5)
			return NULL;
		varStart = p->random_variable(s);
		i++;
	}
	int col1 = s->config[varStart];
	int col2 = s->config[p->connections[varStart][rand()%p->connections[varStart].size()]];
	map<int,int> chain;
	stack<int> openlist;
	openlist.push(varStart);
	while (!openlist.empty())
	{
		int v = openlist.top();
		chain[v] = v;
		for (vector<int>::iterator it=p->connections[v].begin(); it !=p->connections[v].end(); ++it)
		{
			int col = s->config[*it];
			if ((col == col1 || col == col2) && (chain.find(v)==chain.end()))
			{
				openlist.push(*it);
			}
		}
		openlist.pop();
	}
	
	for (auto it=chain.begin(); it!=chain.end();++it)
	{
		CSPMove* m = (CSPMove*) p->create_move();
		m->variable = it->first;
		m->value = (s->config[m->variable] == col1) ? col2 : col1;
		m->valuation = p->move_evaluation(s,m);
		p->move_execution(s,m);
	}
	s->valuation = p->config_evaluation(s);
	return s;
}



