#include "incop.h"

#ifndef __BASICVNS_H__
# define __BASICVNS_H__



/* ------------------------- Struct Voisinage ---------------------- */

class AbstractNeighborStructure
{
	public:
		virtual Configuration* shake(OpProblem* problem, Configuration* s)=0;
//		virtual Configuration* firstImprovement(OpProblem* problem, Configuration* s)=0;

};

class PFlip : public AbstractNeighborStructure
{
	public:
		int p;
		PFlip(int p) : p(p){};
		Configuration* shake(OpProblem* problem, Configuration* s);
//		Configuration* firstImprovement(OpProblem* problem, Configuration* s);
};

class Swap : public AbstractNeighborStructure
{
	public:
		Swap() {};
		Configuration* shake(OpProblem* problem,Configuration* s);
//		Configuration* firstImprovement(OpProblem* problem, Configuration* s){return s;};
};

class TwoExchange : public AbstractNeighborStructure
{
	public:
		TwoExchange() {};
		Configuration* shake(OpProblem* problem, Configuration* s);
};

class KempeChain : public AbstractNeighborStructure
{
	public:
		KempeChain() {};
		Configuration* shake(OpProblem* problem, Configuration* s);
};

/* ----------------------------------------------------------------- */

/* ----------------------------- BVNS ----------------------------- */
class BVNSAlgorithm : public IncompleteAlgorithm
{
	public:
		// Nombre max de mouvement (on prend au plus les kmax premiers mouvements de la liste)
		int kmax;
		// Max time required. Algorithm will stop when currentTime - startTime will be superior or equal to this value.	
		int maxTime;
		// Start time of the BVNS ALgorithm
		time_t startTime;
		// Current TIme of the system
		time_t currTime;
		Configuration* previous;
		//~ Configuration* s;
		LSAlgorithm* walkalgo;
		// List Of Movement
		std::vector<AbstractNeighborStructure*> movements;

		NeighborhoodSearch* nbhsearch;
		BVNSAlgorithm(int k, int maxTime);
		~BVNSAlgorithm()
		{
			delete previous;
			while (!movements.empty())
			{
				movements.pop_back();
			}

		};

		void run (OpProblem *problem,Configuration** initSolution);
		/** walk for a particule */
		/*void randomwalk (OpProblem* problem, Configuration* configuration);
		void initthreshold(Configuration** population, int popsize);
		int neighborhoodChange(Configuration* s, Configuration* t, int i);
		Configuration* firstImprovement(Configuration* s);
		Configuration* shake(Configuration* s,int i);*/
};

/* ----------------------------------------------------------------- */


#endif

