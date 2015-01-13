class CarseqMove;

/* le pb d'ordonnancement d'une chaine de montage de voitures traité en Max-CSP : on compte pour chaque option le nombre de 
séquences violant la contrainte de l'option (au plus maxcars[i] pour blocsize[i] ) */
/** the carsequencing problem as a MaxCSP : for each option i, the cost is the number of sequences violating the
option capacity constraint  (at most maxcars[i] for blocsize[i] cars in a sequence)*/
class Carseq: public CSProblem
{ public :
  Carseq(int nbcars,int nbclasses);
/* le nombre d'options */
/** number of options */
  int nbopt;
/* le nombre de classes de voitures */
/** number of car classes */
  int nbclasses;
/* tableau des capacités par option : nombre maximum de voitures (maxcars) dans une séquence de taille (blocsize) */
  int* maxcars;
/* tableau des capacités par option : nombre maximum de voitures (maxcars) dans une séquence de taille (blocsize) */
  int* blocsize;
  int* nbcarsinclass;
  int** optionsinclass;
  int* maxblocsize;
  int blocsizemax;
  int config_evaluation(Configuration * Configuration);
  int config_verification(Configuration * Configuration);
  void init_domaines(int nbvar, int s );
  Configuration* create_configuration();
  void random_configuration(Configuration* configuration);
  int random_conflict_variable(Configuration* configuration);
  Move* create_move();
  void next_move (Configuration* configuration, Move* move, NeighborhoodSearch* nbhs);
  void move_execution(Configuration* configuration,Move* move);
  int move_evaluation (Configuration* configuration,Move* move);
  void update_conflicting_variables (Configuration* configuration,Move* move);
  void best_config_analysis ();
  void adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors);
  void compute_var_conflict(Configuration* configuration);
  int tabuindex(Move* move,Configuration* configuration);
 int tabuinverseindex(Move* move, Configuration* configuration);
void best_config_verification();
int nbconflicts_partialconfig(Configuration* configuration,int length, int carclass);
int chose_bestpermutation(Configuration* configuration, CarseqMove* move);

int partialconflicts(Configuration* configuration,int var1,int var2);
int partialinitconflicts(Configuration* configuration,int var1,int var2);
};



/* Configuration pour le pb d'ordonnancement de voitures : une configuration est codée comme une permutation */
/** Configuration for car sequencing problem  : a configuration is coded as a permutation */
class CarseqConfiguration : public Configuration
{public :
int nbopt;
int nbcarclasses;
int trynumber;
// int* optionconflicts;
int* conflictingvar;
int** tabconflicts;
CarseqConfiguration(int nbvar,int nbopt);
~CarseqConfiguration();
void copy_element ( Configuration* config2);
void init_conflicts();
};

/* Mouvement : un échange de deux voitures de classes différentes */
/** a move : un swap of two cars of different classes */
class CarseqMove: public SwapMove
{public:
int nbopt;
int nbvar;
// int* optionconflicts;
// int* conflictingvar;
CarseqMove(int nbopt,int nbvar);
~CarseqMove();
// void copymove(Move* move1);
int blocsizemax(Configuration* configuration, Carseq* problem);

};


Carseq* carseqproblem(ifstream& file);

int carseq(int argc, char** argv,int tuningmode);