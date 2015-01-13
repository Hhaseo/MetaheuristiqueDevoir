/**
CSP binaires avec des contraintes données en extension : 
ils sont utilisés pour implanter les CSP aléatoires (randommain.cc)

classe ExtensionBinaryCSP */

/* Binary CSPs with constraints given in extension : used for implementing randomcsp (see randomain.cc) */




class ExtensionBinaryCSProblem : public BinaryCSProblem
{ public :

  ExtensionBinaryCSProblem(int nvar, int nconst, int domain);
/** le tableau constraint2 (numéro contrainte nc, indexvali, indexvalj) indique si le couple (vali,valj) de la contrainte nc
est un couple incompatible (valeur 1) ou compatible (valeur 0) */

/* 3 dimension array constraint2 (constraintnumber nc, indexvali, indexvalj) indicates if the couple (vali,valj) 
of constraint nc is forbidden (value 1) or valid (value 0) */
  int*** constraint2;
  int config_evaluation(Configuration* configuration);
  void incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
  int compute_conflict(Configuration* configuration, int var , int val);
  void solution_write();
  Configuration* create_configuration ();
};



void extensionbinary_setconnexions(int nbvar, vector<int>* connexions, int** constraint1);
int*** extensionbinary_tupledatastructure (int nbconst, int s);
void extensionbinary_inittuples(int nbconst, int s, int*** constraint2, int tabu);
