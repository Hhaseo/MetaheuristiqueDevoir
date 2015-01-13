/* le pb des N-reines en MAX-CSP : voisinage standard */
/** the N-queen problem coded as Max-CSP : standard CSP neighborhood */
class Nqueen: public BinaryCSProblem
{ public :
  Nqueen(int nvar);
  int config_evaluation(Configuration * Configuration);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  void init_domaines(int nbvar, int s );
  Configuration* create_configuration();
};

Nqueen* probleme_reines(int nbvar);
Nqueen* queen_problem_creation(int nbvar);

/* le pb des N-reines avec voisinage de type échange (SwapMove) */
/** the N-queen problem with a exchange type move (SwapMove) neighborhood */
class SwNqueen : public Nqueen
{ public :
  int var_conflict_size;
  SwNqueen(int nvar);
  int config_evaluation(Configuration * Configuration);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  virtual void update_conflicts1(Configuration* configuration,Move* move);
  void move_execution(Configuration* configuration, Move* move);
  Move* create_move();
  void next_move(Configuration* configuration, Move* move, NeighborhoodSearch* nbhs);
  void random_configuration(Configuration* configuration);
  int move_evaluation(Configuration* configuration,Move* move);
  void adjust_parameters(Configuration* configuration, int& maxneighbors, int& minneighbors);
  int random_conflict_variable(Configuration* configuration);
  void compute_var_conflict(Configuration* configuration);
  int compute_var_conflict_size(Configuration* configuration);
};

SwNqueen* probleme_swreines(int nbvar);
SwNqueen* swqueen_problem_creation(int nbvar);


/* le pb des N-reines avec voisinage de type échange (SwapMove) 
sans structures de données incrémentales : le plus efficace */
/** N-queen Problem with SwapMove and without any incremental datastructure for evaluation : the most efficient implementation */

class SwNiNqueen : public SwNqueen
{ public :
  SwNiNqueen(int nvar);
  int config_evaluation(Configuration * Configuration);
  int move_evaluation(Configuration* configuration,Move* move);
  void adjust_parameters(Configuration* configuration, int& maxneighbors, int& minneighbors);
  Configuration* create_configuration();

};

SwNiNqueen* probleme_swnireines(int nbvar);
SwNiNqueen* swniqueen_problem_creation(int nbvar);


