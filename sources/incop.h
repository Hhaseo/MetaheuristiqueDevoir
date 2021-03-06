#include <ctime>
#include <vector>
#include <set>
#include <string>

/* Les d�finitions des classes de la partie algorithme + OpProblem */
/** the definitions of classes of the algorithmic part + OpProblem */

/* les classes "abstraites" utilis�es dans les param�tres des m�thodes */
class OpProblem;
class IncompleteAlgorithm;
class Metaheuristic;
class NeighborhoodSearch;
class Move;

#ifndef __INCOP_H__
# define __INCOP_H__

/* la classe Configuration  le champ config comprend la configuration elle-m�me sous forme de tableau d'entiers
le champ valuation contient sa valeur pour l'�valuation */

/** the main class Configuration */

class Configuration

{public :
  int nbvar;
 int trynumber;
/* les valeurs courantes des variables : implant� sous forme de tableau d'entiers*/
/** the current values of the variables : implemented with an array of integers*/
  int* config;
/* la valeur de la configuration */
/** the configuration value */
  int valuation;
  int var_conflict_size;
/* les variables participant � un conflit : implant� sous forme de vecteur */
/** the variables taking part to a conflict : implemented with a vector */
   std::vector<int> var_conflict;
   std::set<int> set_var_conflict;
/* indicateur si la configuration a �t� regroup�e (pour GWW) */
/** indicates if the configuration has been regrouped before (for GWW) */
  int regrouped;
  virtual ~Configuration();
  Configuration();
  Configuration(int nbvar);
/* copie d'une configuration config2 dans this*/
/** copy a configuration config2 into this */
  virtual void copy_element (Configuration* config2);
/* initialisation � 0 de la structure de donn�es des conflits */
/** initialization to 0 of the conflict datastructure */
  virtual void init_conflicts ();
/* stockage de l'augmentation des conflits de (var,val) de incr */
/** store the conflict of (var,val) incremented by incr */
  virtual void incr_conflicts (int var, int val, int index, int incr);

/* stockage du nombre des conflits nbconf de (var,val)  */
/** store the number of conflicts nbconf of (var,val) in the conflict datastructure */
  virtual  void set_conflicts (int var, int val, int index, int nbconf);

/* nombre de conflits de (var,val) stock� */
/** get the number of conflicts (var,val) stored in the conflict datastructure*/
  virtual  int get_conflicts (int var, int val, int index);
/* nombre de conflits de (var,val) , au besoin recalcul� */
/** get the number of conflicts of (var,val), computed if not stored */
  virtual  int get_conflicts_problem (OpProblem* problem, int var, int val);

/* mise � jour des conflits apr�s avoir effectu� le mouvement move*/
/** update the conflict datastructure after a move is done */
  virtual void update_conflicts(OpProblem* problem, Move* move);
};

/* CSPConfiguration : pour les CSP */
/** CSPConfiguration : for the CSPs */
class CSPConfiguration: public Configuration
{
 public :
 int domainsize;

 CSPConfiguration(int nbvar, int domsize);
};

/* L'incr�mentalit� avec stockage de la participation � l'�valuation des valeurs courantes des
variables de la configuration : implant� dans tabconflicts (tableau  � une dimension) */
/** Incremental evaluation with storage in the conflict datastructure
tabconflicts the participation of the current values of the configuration */
class IncrCSPConfiguration : public CSPConfiguration
{ public :
  int* tabconflicts;
  IncrCSPConfiguration (int nbvar);
  IncrCSPConfiguration(int nbvar, int nbcol);
  ~IncrCSPConfiguration();
  void copy_element (Configuration* config2);
  void init_conflicts ();
  void incr_conflicts (int var, int val , int index, int incr);
  void set_conflicts (int var, int val, int index,  int nbconf);
  int get_conflicts (int var, int val , int index);
  int get_conflicts_problem (OpProblem* problem, int var, int val);
  virtual  void set_variableconflicts (int var, int nbconf);
  void update_conflicts(OpProblem* problem, Move* move);
};

/* l'incr�mentalit� totale : participation � l'�valuation de chaque
valeur de chaque variable stock�e dans le  tableau tabconflicts � deux dimensions (variable, indice de la valeur)*/
/** Full incremental evaluation : the participation of every value of every
variable is stored in the 2 dimension array tabconflicts (variable, valueindex)
*/
class FullincrCSPConfiguration : public CSPConfiguration
{ public :
  int tabconflictsize;
  int** tabconflicts;

  FullincrCSPConfiguration(int nbvar, int domainsize);
  ~FullincrCSPConfiguration();
  void copy_element (Configuration* config2);
  void init_conflicts ();
  void incr_conflicts (int var, int val , int index, int incr);
  void set_conflicts (int var, int val, int index, int nbconf);
/* nombre de conflits de (var,val) stock� : utilisation de l'indice de la valeur index*/
/** get the number of conflicts (var,val) stored in the conflict datastructure using the value index in the domain */
  int get_conflicts (int var, int val, int index);
  int get_conflicts_problem (OpProblem* problem, int var, int val);
  void update_conflicts(OpProblem* problem, Move* move);
};

/* classe Move */
/** root class Move */
class Move
{public :
 int valuation;
 Move();
 virtual ~Move() {;};
/* le test d'�galit� d'un mouvement (utilis� pour la recherche d'un mouvement dans la liste taboue)*/
/** the test of equality of a move (used for searching a move in the tabu list) */
 virtual int eqmove(Move* move1);
/* copie du mouvement move1 dans this */
/** copy of move move1 into this */
 virtual void copymove(Move* move);
/* le mouvement a mettre dans la liste taboue */
/** the move to be put in the tabu list (to be implemented in the subclasses)*/
 virtual Move* computetabumove(Configuration* config){return 0;};
};

/* classe CSPMove :  un mouvement pour les CSP : variable , valeur */
/** class CSPMove : a classical move for a CSP : variable, value */
class CSPMove : public Move
{public :
 int variable;
 int value;
 CSPMove();
 ~CSPMove() {;};
 int eqmove(Move* move);
 void copymove (Move* move);
/* le mouvement stock� tabou est le mouvement inverse du mouvement effectu� */
/** the move stored is the inverse of the move done */
 Move* computetabumove(Configuration* config);
};


/* classe racine des probl�mes d'optimisation (minimisation) */
/** Root class of Optimization Problems (minimization) */

class OpProblem
{public :
/* la meilleure configuration trouv�e */
/** the best configuration found */
  Configuration* best_config;
/* nombre de variables */
/** the number of variables */
  int nbvar;
/* taille maximum des domaines */
/** maximum domain size */
  int domainsize;
/* borne inf�rieure donn�e au d�part : sert de condition d'arr�t quand elle est atteinte */
/** given lower bound , is used as a stop condition when it is reached */
  int lower_bound;
/* le mouvement courant */
/** the current move being tested */
  Move* currentmove;
/* le premier mouvement faisable essay� dans le voisinage*/
/** the first feasible move tried in the neighborhood */
  Move* firstmove;
/* le meilleur mouvement essay� */
/** the best move found in the neighborhood */
  Move* bestmove;
  OpProblem(){};
  virtual ~OpProblem(){};
/* ex�cution d'un mouvement (modification de la configuration courante) */
/** move execution (modification of the current configuration) */
  virtual void move_execution(Configuration* configuration, Move* move);
/* mise � jour de la structure des conflits (cas IncrCSPConfiguration) */
/** update of  the conflict data structure (case IncrCSPConfiguration) */
  virtual void incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move){};
/* mise � jour de la structure des conflits (cas FullincrCSPConfiguration) */
/** update of  the conflict data structure (case FullincrCSPConfiguration) */
  virtual void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move){};
/* cr�ation des 3 objets Move (currentmove,bestmove,firstmove) */
/** creation of 3 Move objects (currentmove,bestmove,firstmove) */
  virtual void allocate_moves();
/* cr�ation d'un mouvement (la classe du mouvement d�pend du probl�ne) : m�thode implant�e dans les sous-classes */
/** creation of 1 Move object (the class of the Move depends on the problem) : this method
is implemented in subclasses */
  virtual Move* create_move(){return 0;};
/* ajustement des param�tres du voisinage (quand la taille du voisinage est sup�rieure � maxneighbors) */
/** adjustment of the neighborhood parameters (when the size of the actual neighborhood is greater than maxneighbors) */
  virtual void adjust_parameters(Configuration* configuration, int & maxneighbors, int & minneighbors){};
/* prochain mouvement du voisinage � tester */
/** next move to be tested (implemented in subclasses)*/
  virtual void next_move(Configuration* configuration, Move* move, NeighborhoodSearch* nbhs){};
/* affectation al�atoire des variables d'une configuration */
/** random assignment of the variables of a configuration */
  virtual void random_configuration(Configuration* configuration){};
/* analyse da la meilleure solution */
/** analysis of the best configuration */
  virtual void best_config_analysis(){};
/* ecriture de la meilleure solution */
/** writing the best solution */
  virtual void best_config_write(){};

/* v�rification de la meilleure solution (recalcul de son co�t) */
/** verification of the best solution (its cost is recomputed) */
  virtual void best_config_verification();
/* initialisation d'une population de taille populationsize */
/** initialization of the population of size populationsize */
  virtual void init_population (Configuration** population,int populationsize) {};
/* cr�ation d'une configuration (la classe exacte d�pend du probl�me) */
/** create a configuration (the exact class depends on the problem and must defined in subclasses) */
  virtual Configuration* create_configuration(){return 0;};
/* calcul de la participation � l'�valuation de l'affectation (var,val) */
/** computation of the participation of (var,val) to the configuration evaluation */
  virtual int compute_conflict (Configuration* configuration,int var, int val) {return 0;};
/* �valuation d'une configuration */
/** evaluation of a configuration */
  virtual int config_evaluation(Configuration* configuration) {return 0;};
/* �valuation d'un mouvement move sur une configuration */
/** evaluation of a configuration if the move is done */
  virtual int move_evaluation (Configuration* configuration,Move* move){return 0;};
/* passage de l'indice dans le domaine � la valeur */
/** valueindex in the domain to value */
  virtual int index2value (int index, int var) {return index;};
/* passage d'une valeur � son indice dans le domaine de la variable */
/** valueindex of value in its domain*/
  virtual int value2index(int value,int var) {return value;};
/* calcule l'ensemble des variables en conflit de la configuration*/
/** compute the variables participating to a conflict in the configuration */
  virtual void compute_var_conflict (Configuration* configuration) {};
  virtual int tabuindex(Move* move, Configuration* configuration) {return 0;};
  virtual int tabuinverseindex(Move* move, Configuration* configuration){return 0;};
  virtual int nbtabuindex(){return 0;};
};

/* Le voisinage param�tr� d'une part par  min voisins explor�s,
max voisins explor�s et �puisement voisinage et d'autre part par var_conflict et val_conflict */
/** Class NeighborhoodSearch : how the neighborhood is explored */

class NeighborhoodSearch
{public :
/* nombre minimum de voisins explor�s */
/** minimum number of visited neighbors */
  int minneighbors;
/* nombre maximum de voisins explor�s */
/** maximum number of explored neighbors */
  int maxneighbors;
/* indicateur de comportement quand le voisinage est �puis� sans qu'un voisin n'ait �t� accept� :
0 stagnation, 1 on effectue le 1er mouvement faisable, k on effectue le meilleur mouvement faisable parmi k mouvements essay�s non accept�s*/
/** behavior indicator when the neighborhood is exhausted and no neighbor has been accepted :
0 stagnation, 1 the 1st feasible move is selected, k the best feasible among k tried but not accepted moves is selected */
  int finished;
/* indicateur de restriction aux variables en conflit (0 pas de restriction, 1 restriction) */
/** restriction indicator to variables participating in a conflict (0 no restriction, 1 restriction) */
  int var_conflict;
/* indicateur de restriction aux meilleures variables d'une variable (0 pas de restriction, 1 restriction) */
/** restriction indicator to best values of a variable (0 no restriction , 1 restriction) */
  int val_conflict;
  double nbhrate;
  NeighborhoodSearch( int maxneigh, int minneigh, int finish, int var_conf, int val_conf, double nbbr);
  int returnbestmove();
  void adjust_neighborhood(Configuration* configuration, OpProblem* problem, int& maxneigh, int& minneigh, int nbmoves);
  virtual void dynamicmaxneighbors(int & maxneigh, int & minneigh, int nbmoves);
  virtual void initsearch();
  virtual void spareneighboradjust(Configuration* config, Move* move){;}
};


/* Voisinage avec r�glage dynamique du param�tre max-voisins*/
/** Neighborhood with dynamic parameter tuning */
class DynamicNeighborhoodSearch: public NeighborhoodSearch
{public:
 DynamicNeighborhoodSearch(int maxneigh, int minneigh, int finish, int var_conf, int val_conf, double nbbr);
/* valeur initiale du parametre maxneighbors */
/** initial value of maxneighbors parameter */
 int initmaxneighbors;
/* valeur initiale du parametre minneighbors */
/** initial value of minneighbors parameter */
 int initminneighbors;
/* p�riode de r�ajustement du param�tre */
/** parameter readjustment period */
 int adjustperiod;
 void initsearch();
/* ajustement des param�tres minneighbors et maxneighbors */
/** adjust the parameters maxneighbors and minneighbors */
 void dynamicmaxneighbors(int & maxneigh, int & minneigh, int nbmoves);
};

class DynamicSpareneighbor: public NeighborhoodSearch
{public :
DynamicSpareneighbor (int maxneigh, int minneigh, int finish, int var_conf, int val_conf, double nbbr);
void spareneighboradjust(Configuration* config, Move* move);
int nbmovesdown;
};

/* Les Algorithmes
la classe mere : algo de recherche incomplet */
/** Root class of algorithms */

class IncompleteAlgorithm
{public :
  std::string methodname;
/* un seuil peut �tre utilis� pour emp�cher des mouvements de co�t sup�rieur au seuil
(utilis� dans les recherches locales des marches de GWW)*/
/** a threshold can be used to forbid moves above this threshold (used in LSAlgorithms implementing walks inside GWW)*/
  int threshold;
  virtual ~IncompleteAlgorithm(){};
/* marche d'une particule */
/** walk for a particule */
  virtual void randomwalk (OpProblem* problem, Configuration* configuration);
  virtual void initthreshold(Configuration** population, int popsize){;};
/* ex�cution de l'algorithme sur une population (r�duite � une particule pour une recherche locale) */
/** Run the algorithm on a population (array of configurations) */
  virtual void run (OpProblem *problem,Configuration ** population);

};

/* la classe des algos de marche al�atoire param�tr�e par longueur marche
un voisinage et une metaheuristique */
/** The class of local search algorithm on one particle : the random walk is
parameterized with the walk lengh,a neighborhood and a metaheuristics */

class LSAlgorithm: public IncompleteAlgorithm
{public :
/* longueur de la marche */
/** walk length */
  int walklength;
/* le voisinage */
/** the way the neighborhood is explored */
  NeighborhoodSearch * nbhsearch;
/* la m�taheuristique */
/** the metaheuristics used */
  Metaheuristic* mheur;
/* le nombre d'essais de mouvements (pour les stats) */
/** number of move tries  (for statistics) */
  int nhtries;
  double avgnhtries;
  double avgsqnhtries;
/* nombre de mouvements effectu�s */
/** number of moves done */
  int nbmoves;
  LSAlgorithm(int nbmov);
  ~LSAlgorithm();
/* faisabilit� d'un mouvement  (sous ou au niveau du seuil pour marche de GWW) */
/** feasability of a move (under or at threshold level pour GWW walks) */
  virtual int isfeasible(Move* move);
  void randomwalk   (OpProblem* problem, Configuration* configuration);
/* algorithme d'exploration du voisinage pour s�lectionner et effectuer un mouvement � partir de la configuration courante
Effectue le mouvement et renvoie 1 si un mvt a ete effectu� et 0 si aucun mouvement ne l'a �t�*/
/** Neighborhood exploration algorithm for selecting and do a move from the current configuration :
returns 1 if a move has been done and 0 if no move has been done */
  virtual int configurationmove(OpProblem* problem,Configuration* configuration);
  void initthreshold(Configuration** population, int popsize);
  void run (OpProblem *problem, Configuration ** population);
/* test de meilleur trouv� (renvoie 1 si un meilleur absolu est trouv�)*/
/** test if a global best configuration has been found (returns 1 in that case) */
  int test_bestfound(Move* move);


};

class LSAlgorithmGWW: public LSAlgorithm
{public :
 LSAlgorithmGWW(int nbmov);
 int isfeasible(Move* move);
};

/* les diff�rentes m�taheuristiques */
/** Root class for Metaheuritics */
class Metaheuristic
{public :
  virtual ~Metaheuristic(){};
/* mise � jour des donn�es de la m�taheuristique juste avant qu'un mouvement soit effectu� */
/** update of the metaheuristic data just before a move is performed */
  virtual void executebeforemove(Move* move, Configuration* configuration,OpProblem* problem);
/* initialisation des donn�es de la m�taheuristique */
/** initialization of the meteheuristic data at the beginning of a local search */
  virtual void reinit(OpProblem* problem);
/* condition d'acceptation d'un mouvement : renvoie 1 si le mouvement est accept� */
/** acceptance condition of a move : returns 1 if the move is accepted */
  virtual int acceptance(Move* move,Configuration*  config);
  virtual void adjustparameter (int parameter) {;};
};

/* marche avec liste taboue : parametree par longueur de la liste : cette liste de mouvements est
implantee � l'aide d'une liste de Move* */
/** Walk with using a tabu list : this list of moves is implemented by a list<Move*> structure , the
actual class of the moves depend on the problems */
class TabuSearch: public Metaheuristic
{public :
/* longueur maximale de la liste taboue */
/** maximum length of the tabulist */
  int tabulength;
/* liste taboue : trait�e comme une file */
/** tabu list : implemented FIFO */
  std::list<Move*> move_list;
  TabuSearch(int tabul);
/* acceptation d'un mouvement : non tabou  (le crit�re d'aspiration est dans l'algo de recherche du voisin) */
/** acceptance of a move : not in the tabulist (the aspiration criterion of a best is in the configurationmove algorithm) */
  int acceptance (Move* move, Configuration* config);
/* test de non pr�sence dans la liste taboue : la pr�sence d'un mvt est faite avec eqmove */
/** test of non presence in the tabulist (use of eqmove method) */
  int  nontabumove (Move* move);
/* mise � jour de la liste taboue qui est trait�e comme une file de longueur maximale tabulength */
/** updating of the tabulist which is managed as a FIFO of maximum length tabulength */
  void executebeforemove(Move* move, Configuration* configuration, OpProblem* problem);
/* r�initialisation : la liste taboue est vid�e */
/** the tabu list is cleared */
  void reinit(OpProblem* problem);
  void adjustparameter (int length);
};

class TabuGreedySearch: public TabuSearch
{public :
  TabuGreedySearch(int tabul);
 int acceptance (Move* move, Configuration* config);
};

class IncrTabuSearch: public TabuSearch
{public :
  IncrTabuSearch(int tabul);
  int nbiter;
  std::vector<int> tabutime;
  OpProblem* currentproblem;
  int  acceptance (Move* move, Configuration* config);
  void executebeforemove(Move* move, Configuration* configuration, OpProblem* problem);
  void reinit(OpProblem* problem);
};

class IncrTabuGreedySearch: public IncrTabuSearch
{public :
  IncrTabuGreedySearch(int tabul);
 int acceptance (Move* move, Configuration* config);
};

/* marche Metropolis : un seul param�tre = temperature */
/** Metropolis algorithm : a unique parameter - a constant temperature */
class Metropolis: public Metaheuristic
{public :
  double temperature;
  Metropolis(double temp);
/* la formule classique de Metropolis d'acceptation d'un mouvement d�t�riorant
l'�valuation : probabilit� p = exp (-evaluationdelta/temperature) */
/** the classical Metropolis formula for accepting a bad move :  probability =  exp (-evaluationdelta/temperature) */
  int acceptance (Move* move, Configuration* config);
  void adjustparameter(int parameter);
};

/* l'acceptation � seuil : un mouvement ne doit pas d�t�riorer l'�valuation plus que le seuil courant ;
le seuil diminue lin�airement de thresholdinit � 0*/

/** Threshold accepting Metaheuristics : a move must no deteriorate the evaluation more than the
current threshod : the threshold goes down linearly from thresholdinit to 0 */
class ThresholdAccepting: public Metaheuristic
{public :
/* seuil initial */
/** initial threshold */
  double thresholdinit;
/* pas de baisse du seuil */
/** constant step to lower the threshold */
  double delta;
/* valeur courante du seuil */
/** current value of the threshold */
  double thresholdaccept; // le seuil tel que g�r� par TA
/* constructeur : 2 arguments seuil initial maxthreshold et nombre de pas,
le pas constant delta de baisse du seuil est calcul�*/
/** constructor : two arguments : maxthreshold the initial threshold and
walklength , it computes a constant step for lowering the threshold */
  ThresholdAccepting(double maxthreshold, int walklength);
/* condition d'acceptation : �tre sous ou au niveau du  seuil */
/** acceptance condition : being under or at the threshold */
  int acceptance (Move* move,Configuration* config);
/* le seuil est diminu� de delta */
/** the threshold is lowered by delta */
  void executebeforemove(Move* move, Configuration * configuration, OpProblem* problem);
/* le seuil est initialis� � thresholdinit */
/** the threshold is initialized at thresholdinit */
  void reinit(OpProblem* problem);
};

/* le recuit simul� : descente lin�aire de temp�rature de inittemperature � 0 */
/** Simulated Annealing : linear temperature descent from inittemperature to 0*/
class SimulatedAnnealing: public Metaheuristic
{public :
/* temperature initiale */
/** initial temperature */
  double inittemperature;
/* pas constant de baisse de temperature */
/** constant step for lowering the temperature */
  double delta;
/* temperature courante */
/** current temperature */
  double temperature;
  int walklength;
/* Constructeur : 2 arguments : temp�rature initiale et longueur de marche */
/** Constructor : 2 parameters  : initial temperature and walk length : the fixed
temperature decrement is computed. */
  SimulatedAnnealing(double initialtemperature, int walklength);
/* acceptation en fonction de la temperature : formule classique du recuit simul�
probablit� d'acceptation d'un mouvement d�t�riorant l'�valuation :
probabilit� =  exp (-evaluationdelta/temperature) */

/** Acceptance function of the temperature : classical simulated annealing formula
for accepting a bad move :  probability =  exp (-temperature/evaluationdelta) */
  int acceptance (Move* move, Configuration* config);
/* la temp�rature est baiss�e de delta */
/** the temperature is lowered by delta */
  void executebeforemove(Move* move, Configuration * configuration, OpProblem* problem);
  void reinit(OpProblem* problem);
  void adjustparameter (int parameter);
};


/* marche hybride tabou + pourcentages d'acceptation selon sens des mouvements */
/** Special Tabu search with complementary acceptance condition depending on the move direction */


//                          liste taboue

class TabuAcceptingrate: public TabuSearch
{public :
   /* probabilit� d'acceptation d'un mauvais   */
  /** probability of acceptance of a worsening move */
  float Pd;
  /* probabilit� d'acceptatiion d'un mouvement de m�me co�t que le courant */
  /** probability of acceptance of a move with same cost */
  float P0;
  TabuAcceptingrate(int tabul, float Pd, float P0);
/* crit�re d'acceptation : non tabou et pourcentages d'acceptation suivant sens du mouvement (d�t�riorant, neutre, am�liorant) */
/** Acceptance condition : non tabu and probabilities depending on the move direction */
  int acceptance (Move* move, Configuration* config);
};


/* marche al�atoire : tout voisin faisable est accept� */
/** Random walk : every feasible neighbor is accepted */
class RandomSearch: public Metaheuristic
{public :
  RandomSearch();
  int acceptance (Move* move, Configuration* config);
};

/* marche gloutonne : on accepte un voisin de cout inf�rieur ou �gal � la configuration courante*/
/** Greedy walk : a neighbor with better or same cost as the current configuration is accepted */
class GreedySearch: public Metaheuristic
{public :
  GreedySearch();
  int acceptance (Move* move, Configuration* config);
};

//-------------------------------------------------------------------------------------------------


/* les algos de type GWW
 les diff�rentes sous classes diff�rent par la gestion du seuil
et les regroupements de particules */

/** the GWW (Go with the winners) algorithms : the different subclasses
differ by the way a threshold is managed and the particles are regrouped */

class GWWAlgorithm: public IncompleteAlgorithm
{public :
/* nombre de particules */
/** number of particles */
  int populationsize;
/* indicateur de marche uniquement si la particule a �t� regroup�e
(utile pour GWW de base, sans recherche locale, uniquement) (1 oui, 0 non) */
/** walk indicator : a walk is performed only is the particle has been regrouped : (1 yes, 0 no)
(useful for a standard GWW with random walk (and no local search)) */
  int regrouptest;
/* indicateur de baisse du seuil au dernier mouvement de la marche (pour essayer d'empecher la particule d' etre redistribu�e) (1 oui, 0 non) */
/** parameter if the threshold is lowered at the last move of the walk
(for trying to avoid the particle to be redistributed  (1 yes, 0 no)*/
  int lastmovedescent;
/* indicateur d'�litisme : remet-on le meilleur individu dans la population � chaque regroupement (1 oui, 0 non) */
/** elitism parameter : is the best particle put again in the population at each regroupment ( 1 yes, 0 no) */
  int elitism;
/* indicateur d'arr�t de la marche en cas de stagnation (1 oui, 0 non) */
/** parameter for stopping the walk in case of stagnation (1 yes, 0 no) */
  int nomovestop;
/* le d�cr�ment du seuil (calcul� par thresholdcomputedelta) */
/** the threshold decrement (compted by thresholdcomputedelta) */
  int thresholddelta;
/* le nombre d'iterations max : utile quand pas de seuil (NothresholdGWWAlgorithm) */
/** the maximum number of iterations : useful when no threshold is managed (NothresholdGWWAlgorithm) */
  int nbiteration;
/* le nombre de changements de seuil (pour les statistiques) */
/** number of threshold changes (for the statistics) */
  int thresholdchanges;
/* le nombre total d'essais de mouvements entre 2 regroupements (pour les statistiques)*/
/** total number of move tries between 2 regroupments (for the statistics) */
  int total_nhtries;
/* le nombre total de mouvements entre 2 regroupements (pour les statistiques)*/
/** total number of moves between 2 regroupments (for the statistics) */
  int total_nbmoves;
/* l'algorithme de recherche locale utilis� */
/** the local search algorithm used */
  LSAlgorithm* walkalgorithm;
/* destructeur */
  ~GWWAlgorithm();
/* recherche locale sur l'ensemble de la population */
/** local search on the whole population */
  virtual  void populationrandomwalk (OpProblem* problem, Configuration** population);
/* le nombre de particules au seuil (pour les statistiques),  la population �tant d�j� tri�e � l'appel */
/** the number of particles at the threshold (for statistics) , the population being yet sorted at the function call*/
  virtual int nb_threshold_population(Configuration** population);
/* une recherche locale pour une particule */
/** a local search for a particle */
  void randomwalk   (OpProblem* problem, Configuration* configuration);
/* initialisation du seuil */
/** intialization of the threshold */
  void initthreshold(Configuration** population, int popsize);
/* m�thode de baisse du seuil (le delta a d�j� �t� calcul�)*/
/** method for lowering the threshold( the delta has already been computed) */
  virtual void thresholdupdate();
/* m�thode de calcul du d�cr�ment du seuil */
/** method for computing the threshold decrement */
  virtual void thresholdcomputedelta(Configuration** population);
/* d�roulement de l'algorithme */
/** main function for running the algorithm */
  void run (OpProblem *problem, Configuration** population);
/* regroupement des mauvais individus sur les bons */
/** regrouping of the best particles on the good ones */
  virtual  void regrouping(Configuration** population);
/* en cas d'�litisme, on remet le meilleur dans la population */
/** in case of elitism, the best particle is put into the population */
  void populationkeepbest (OpProblem* problem, Configuration** population);
/* incremente le compteur de changements de seuil (pour les statistiques) */
/** incrementing the threshold updates counter (for the statistics) */
  virtual  void thresholdchangesupdate();
};

/* Classe abstraite : GWW avec seuil */
/** Abstract class : GWW managing a threshold */
class ThresholdGWWAlgorithm : public GWWAlgorithm
{public :
  void thresholdupdate();
void thresholdchangesupdate();
void initthreshold(Configuration** population, int popsize);
int nb_threshold_population(Configuration** population);
};


/* GWW standard : descente du seuil avec taux fixe */
/** Standard GWW : threshold descent with a fixed rate */
class StandardGWWAlgorithm : public ThresholdGWWAlgorithm
{public :
/* taux de baisse du seuil */
/** threshold descent constant rate */
  double thresholddescent;
/* seuil minimum (correspond en g�n�ral � une borne inf�rieure connue) */
/** minimum of the threshold (corresponds generally to a known lowerbound) */
  int thresholdmin;
  void regrouping(Configuration** population);
  StandardGWWAlgorithm(int population_size, int grtest,int lastmove, int elitisme,int stop, double thresdescent,int threshmin );
  void thresholdcomputedelta(Configuration** population);
};

/* GWW descente du seuil au moins au niveau du pire */
/** StandardGWW with a threshold descent at least until the worst particle */
class FastStandardGWWAlgorithm: public StandardGWWAlgorithm
{public :
   void thresholdcomputedelta(Configuration** population);
   FastStandardGWWAlgorithm(int population_size, int grtest,int lastmove, int elitisme, int stop, double thresdescent,int threshmin );
};

/* GWW avec descente su seuil en tuant un nombre donn� de particules � chaque fois */
/** GWW with a threshold descent such as a given number of particles is regrouped */
class AdaptiveGWWAlgorithm : public ThresholdGWWAlgorithm
{ public :
/* nombre de mauvaises particules � regrouper sur les bonnes */
/** number of bad particles to be regrouped on good ones */
  int nbkilled;
  AdaptiveGWWAlgorithm(int population_size, int grtest, int lastmove, int elitisme, int stop, int nbkilled );
  void regrouping(Configuration** population);
  void thresholdcomputedelta(Configuration** population);
};

/* GWW avec descente du seuil au plus bas des valeurs de seuil obtenues par AdaptiveGWWAlgorithm et FastStandardGWWAlgorithm
 (un nombre de particules et un taux) */
/** GWW with a threshold descent at the lowest value obtained by AdaptiveGWWAlgorithm et FastStandardGWWAlgorithm
using a number of particles to be redistributed and a rate */
class FastAdaptGWWAlgorithm: public AdaptiveGWWAlgorithm
{public :
/* taux de descente du seuil */
/** threshold descent rate */
  double thresholddescent;
  int nbmaxkilled;
  FastAdaptGWWAlgorithm(int population_size, int grtest, int lastmove, int elitisme, int stop, int nbkilled, int maxkilled, double thresholddescent);
  void thresholdcomputedelta(Configuration** population);
};

/* GWW avec descente du seuil en fonction du m�dian de la population*/
/** GWW with a descent depending on a distance between the worst and the median particle */
class MedianAdaptGWWAlgorithm: public AdaptiveGWWAlgorithm
{public :
/* taux de baisse du seuil : fraction de la distance entre la pire et la m�diane (entre 0 et 1) */
/** descent rate : porcentage of the distance between the worst and the median particles (between 0 and 1) */
  double mediandescent;
  MedianAdaptGWWAlgorithm(int population_size, int grtest, int lastmove, int elitisme, int stop, double mediandescent);
  void thresholdcomputedelta(Configuration** population);
};

/* GWW avec descente du seuil en fonction du meilleur de la population*/
/** GWW with a descent depending on a distance between the worst and the best particle */
class BestAdaptGWWAlgorithm: public AdaptiveGWWAlgorithm
{public :
/* taux de baisse du seuil : fraction de la distance entre la pire et la meilleure (entre 0 et 1) */
/** descent rate : porcentage of the distance between the worst and the best particles (between 0 and 1) */

  double bestdescent;
  BestAdaptGWWAlgorithm(int population_size, int grtest, int lastmove, int elitisme, int stop, double bestdescent);
  void thresholdcomputedelta(Configuration** population);
};



/* GWW sans seuil : 2 param�tres : nombre de tu�s � chaque it�ration, nombre d'it�rations */
/** GWW without threshold management : 2 parameters : number of particles redistributed at each iteration , number of iterations */
class NothresholdGWWAlgorithm : public GWWAlgorithm
{public :
  NothresholdGWWAlgorithm(int population_size, int grtest, int lastmove, int elitisme, int stop,
	int killed,  int nbiter);
  void regrouping(Configuration** population);
/* nombre de particules � regrouper � chaque it�ration */
/** number of particles to be regrouped at each iteration */
  int nbkilled;
};


#endif

