#include <vector>
#include <list>
#include <set>
#include <string>
#include <algorithm>

using namespace std;
#include "incop.h"
#include "move.h"

/* les mouvements de permutation et d'échange  utilisés par les N-reines (queen) et maximum clique (maxclique)
pour les N-reines, il s'agit de permuter les valeurs de 2 variables, pour maxclique d'échanger une variable
dans la clique avec une variable hors la clique.
*/


/* la classe SwapMove */


/**  le test d'égalité de la liste taboue  : ne pas faire un mouvement tabou */
 
int SwapMove::eqmove (Move* move1)
{
  return ( 
	  (((SwapMove *)move1)->variable1== variable1
	   &&  ( (SwapMove*)move1)->variable2 == variable2 )
||
  (((SwapMove *)move1)->variable1== variable2
	   &&  ( (SwapMove*)move1)->variable2 == variable1 )
	  )
    ;
}

/* le mouvement tabou est le mouvement inverse */
Move* SwapMove::computetabumove(Configuration* configuration)
{SwapMove* tabumove = new SwapMove();
 tabumove->variable1 = variable2;
 tabumove->variable2 = variable1;
 return tabumove;
}


void SwapMove::copymove(Move* move1)
{valuation=move1->valuation;
 variable1=((SwapMove*)move1)->variable1;
 variable2=((SwapMove*)move1)->variable2;
}

SwapMove::SwapMove() {valuation=0;variable1=0;variable2=1;}


/* la classe ExchangeMove utilisée par maxclique */

int ExchangeMove::eqmove (Move* move1)
{
  return ( 
	  ((ExchangeMove *)move1)->variable1== variable1
	  );
}

/* le mouvement tabou est le mouvement inverse */
Move* ExchangeMove::computetabumove(Configuration* configuration)
{ExchangeMove* tabumove = new ExchangeMove();
 tabumove->variable1 = variable2;
 tabumove->variable2 = variable1;
 return tabumove;
}


void ExchangeMove::copymove(Move* move1)
{valuation=move1->valuation;
 variable1=((ExchangeMove*)move1)->variable1;
 variable2=((ExchangeMove*)move1)->variable2;
}

ExchangeMove::ExchangeMove() {valuation=0;variable1=0;variable2=1;}
