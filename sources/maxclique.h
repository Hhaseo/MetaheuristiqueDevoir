/* probleme maxclique : on recherche une clique d'une taille donnée dans un graphe : 
en augmentant la taille de la clique recherchée, on résout le problème maxclique ; les graphes sont donnés
dans le format Dimacs  */
/** Clique Problem : one searches for a clique of given size in a graph  : by increasing the clique size, one solves
the classical Maximum clique problem  : the graphs are given in standard Dimacs format*/

/* classe pour la recherche d'une clique de taille cliquesize 
on essaie de trouver une clique de taille n donnée en minimisant les conflits (aretes manquantes
dans l'ensemble des n sommets candidats
Un mouvement est un échange entre un sommet dans la clique candidate participant à un conflit 
et un sommet hors clique (remainvariables)
Utilisation de ExchangeMove défini  dans move.cc  */

/** CliqueProblem class : searching for a clique of size cliquesize  : one minimizes the conflicts (number of missing edges
for variables in clique array) ; the remaining variables are in remainvariables array. The neighborhood is to exchange 
2 variables between the 2 arrays (class ExchangeMove defined in move.h) */

class CliqueProblem : public BinaryCSProblem
{ public :
/* taille de la clique recherchée */
/** size of the clique to be found */
  int cliquesize;
/* les cliquesize variables de la configuration candidates pour former une clique */
/** clique array : the variables in the configuration trying to form a clique */
  int* clique(Configuration* configuration);
/* les variables restantes (pas dans le tableau clique) */
/** remainvariables array : the variables in configuration not in clique array */
  int* remainvariables(Configuration* configuration);
  CliqueProblem(int nvar, int nconst, int clsize);
  int config_evaluation(Configuration*  configuration);
  int config_evaluation_verif(Configuration*  configuration);
  int move_evaluation (Configuration* configuration, Move* move);
  void incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  void init_domains(int nbvar);
  // int compute_conflict (Configuration* configuration,int var, int val);
  //  void compute_var_conflict(Configuration* configuration);
void random_configuration(Configuration* configuration);
void move_execution(Configuration* configuration,Move* move);
void solution_write();
  void next_move (Configuration* configuration, Move* move, NeighborhoodSearch* nbhs);
Move* create_move();
Configuration* create_configuration();
int minconflict_invariable(Configuration* configuration);
void adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors);
};

CliqueProblem* probleme_maxclique(int nbvar,int nbconst,int clsize,int** constraint1);
CliqueProblem* clique_problem_creation (int clsize,ifstream & file);
void lire_debut_fichier_clique(ifstream & file, int& nbvar, int& nbconst);
void lire_fichier_clique (ifstream& file, vector<int>* connexions,int ** constraint1);

/* configuration pour les cliques (on utilise une structure de données pour les conflits)
a ete implanté comme sous-classe de FullincrCSPConfiguration 
(aurait aussi pu etre sous-classe de IncrCSPConfiguration : 1 seule valeur de conflits stockée par variable :
le nombre d'aretes manquantes avec les variables de la clique
champs spécifiques : les tableaux des variables dans et hors clique
*/

/** Configuration for the clique problem (as one uses a  datastructure for the conflicts) : is implemented as a
subclass of FullincrCSPConfiguration (could also have been implemented as a subclass of IncrCSPConfiguration) : only one conflict
value stored by variable : the number of missing edges with the variables in clique array. */

class CliqueConfiguration: public FullincrCSPConfiguration
{ public :
 int cliquesize;
/* les n variables formant la clique candidate de taille n (evaluation : nombre d'aretes manquantes pour être une clique*/
/** the variables in the configuration trying to form a clique : the number of missing edges to form a clique is
the evaluation of the configuration*/
 int* clique;
/* les variables restantes (pas dans le tableau des variables candidates pour former une clique) */
/** remainvariables array : the variables in configuration not in clique array */
 int* remainvariables;
 CliqueConfiguration(int nbvar,int cliquesize);
 ~CliqueConfiguration();
 void copy_element (Configuration* config2);

};

int maxclique (int argc, char** argv, int tuningmode);