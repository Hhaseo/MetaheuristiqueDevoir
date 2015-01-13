/* la classe des problemes d'affectation de fréquences du CELAR  (celar6,7,8) */
/** the CELAR frequency assignment problem class (celar6. celar7 celar8)*/

class CelarCSProblem : public BinaryCSProblem
{public :
  int* numerovariable;
  vector<int> tabsymindex[8];
  int tabsymdomaines[800];
  int* tabindex[8];
  int tailledomaines[8];
  int* distance;
  int* valconst;
  int* refvalue;
  int* refcost;
  int varcost;
  int neighborhoodlength;
  CelarCSProblem(int nvar, int nconst);
  int index2value(int index,int var);
  int value2index(int index,int var);
  int config_evaluation(Configuration* configuration);
  int move_evaluation(Configuration* configuration,Move* move);
  int move_evaluation1(Configuration* configuration,int valeur,int var_changee,int val_changee);
  void inittables(vector<int>* tabledomaines);
  void incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  void incr_update_conflicts_1(IncrCSPConfiguration* configuration,int var_changee, int val_changee);
  void fullincr_update_conflicts_1(FullincrCSPConfiguration* configuration,int var_changee, int val_changee);
  void move_execution(Configuration* configuration, Move* move);
  int compute_conflict(Configuration* configuration,int var,int val);
  void random_configuration(Configuration* configuration);
  void compute_var_conflict(Configuration* configuration);
  Configuration* create_configuration();
  int random_variable(Configuration* configuration);
  int random_conflict_variable(Configuration* configuration);
  void best_config_write ();
  int tabuindex(Move* move, Configuration* configuration);
  int tabuinverseindex(Move* move, Configuration* configuration);

};

CelarCSProblem* probleme_celar(int nbvar,int nbconst,vector<int>* tabdomaines,
			       int * domaines,int **constraint1,int *distance,
			       int * valconst,int *numerovariable, int varcost, int* refvalue, int* refcost );
CelarCSProblem* celar_problem_creation (ifstream & file, int varcost);


void lire_fichier_celar (ifstream& file, int nbvar, vector<int>* connexions,int ** constraint1, int* distance,
int* valconst,int* numerovariable);

void lire_debut_celar(ifstream & file, int & nbvar, int & nbconst);
void lire_suite_celar(ifstream & file,int nbvar,int* domaines,int * numerovariable, int varcost, int* refvalue, int* refcost);

int celarcsp(int argc, char** argv, int tuningmode);