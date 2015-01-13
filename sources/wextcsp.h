/* les CSP en extension binaires valués (poids par contrainte) */
/** Binary CSP with valued constraints (a weigth per constraint) */

class WeightExtensionBinaryCSProblem : public ExtensionBinaryCSProblem
{ public :
/* poids des contraintes . vecteur indicé par le numero de contrainte */
/** constraint weights : vector indexed by the constraint number */
  vector<int> cstweights;
/* vecteur des numéros de contraintes par paire de variables */
/** vector of the constraints connecting a pair of variables (i,j) (i<j) */
  vector<int>** lconstraints;
  WeightExtensionBinaryCSProblem(int nvar, int nconst, int domsize);
  int config_evaluation(Configuration* configuration);
  void incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  int compute_conflict(Configuration* configuration, int var , int val);
  Configuration* create_configuration ();
};

WeightExtensionBinaryCSProblem* weighted_csp(int nbvar, int nbconst, int s, vector<int> ** constraint1, int*** constraint2, vector<int>& wei);
WeightExtensionBinaryCSProblem* weighted_csp_creation (int nbvar, int nbconst, int s, vector<int> ** constraint1, int*** constraint2,  vector<int>* connexions, vector<int>&  wei);
void wextensionbinary_setconnexions(int nbvar, vector<int>* connexions, vector<int>** constraint1);

