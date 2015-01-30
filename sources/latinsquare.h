
/* Problème du carré latin équilibré (cf CPAIOR'04) */
/** Spatially Balanced latin square problem (cf CPAIOR'04)*/
class Latinsquare : public CSProblem
{ public :
  int squaresize;
  Latinsquare(int size);
int config_evaluation(Configuration * Configuration);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  void init_domaines(int nbvar, int s );
  Configuration* create_configuration();
void move_execution(Configuration* configuration,Move* move);
int move_evaluation (Configuration* configuration,Move* move);
Move* create_move();
void next_move (Configuration* configuration, Move* move, NeighborhoodSearch* nbhs);
void random_configuration(Configuration* configuration);
void compute_var_conflict(Configuration* configuration);
void adjust_parameters(Configuration* configuration, int& maxneighbors, int& minneighbors);

};


class SBLatinsquare : public Latinsquare
{ public :
  SBLatinsquare(int size);
int config_evaluation(Configuration * Configuration);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
int move_evaluation (Configuration* configuration,Move* move);}
;

Latinsquare* latinsquare_problem_creation(int size);
SBLatinsquare* sblatinsquare_problem_creation(int size);
Latinsquare* probleme_carrelatin (int nbvar);

/* Mouvement : échange de deux valeurs dans une ligne */
/** Move : swap of 2 values in a line */
class ColSwMove : public Move
{public :
/* ligne modifiée */
/** modified line */
 int line;
/* 1ere variable échangée */
/** 1st swapped variable */
 int variable1;
/* 2eme variable échangée */
/** 2d swapped variable */
 int variable2;
 ColSwMove();
 ~ColSwMove() {;};
int eqmove(Move* move);
void copymove (Move* move);
Move* computetabumove(Configuration* config);
};

void arguments_carre(char** argv, int& narg, int & s);

int latinsquare(int argc, char** argv, int automatic_tuningmode, int sb);
