#include "incop.h"

/* Echange des valeurs de deux variables */
/** Exchange the values of two variables */
class SwapMove : public Move
{public :
 int variable1;
 int variable2;
 SwapMove();
 ~SwapMove() {;};
int eqmove(Move* move);
void copymove (Move* move);
Move* computetabumove(Configuration* config);
};


/* cas particulier d'échange : échange de variables dans et en dehors d'un ensemble (cf maxclique) */
/** particular case of exchange (two variables in and out of a set : used for maxclique problem) */
class ExchangeMove : public SwapMove
{public :
 ExchangeMove();
 ~ExchangeMove() {;};
int eqmove(Move* move);
void copymove (Move* move);
Move* computetabumove(Configuration* config);
};


class FlipMove : public Move
{
public :
	int k;
	OpProblem* problem;
	FlipMove(int k, OpProblem* problem);
    ~FlipMove() {;};
	int eqmove(Move* move);
	void copymove (Move* move);
	Move* computetabumove(Configuration* config);
};
