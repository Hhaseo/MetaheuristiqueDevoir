/* Coloriage de graphe */
/** Graph coloring problems solved as MaxCSP */
class ColorCSProblem : public BinaryCSProblem
{ public :
  ColorCSProblem(int nvar, int nconst, int nbcolor);
  int config_evaluation(Configuration*  configuration);
  void incr_update_conflicts(IncrCSPConfiguration* configuration,Move* move);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
//  int random_variable(Configuration* configuration);
  void init_domains(int nbvar, int s );
//  void init_tabdomains(int s);
  void best_config_analysis();
  void best_config_write();
  void analyse_conflicts(Configuration* configuration);
  void random_configuration(Configuration* configuration);
  Configuration* create_configuration();
  int compute_conflict (Configuration* configuration,int var, int val);
  void compute_var_conflict(Configuration* configuration); 
  int move_evaluation (Configuration* configuration,Move* move);

  int min_conflict_value(int var, int val, Configuration * configuration);
 //int random_conflict_variable (Configuration* configuration);
//  void  adjust_parameters (Configuration* configuration, int& maxneighbors, int& minneighbors);
};





/* Coloriage avec l'algorithme Impasse :  les variables affectées respectent les contraintes ,
une n+1 ème couleur fictive est affectée aux variables qu'on n'a pu affecter sans conflit 
avec une des n couleurs */
/** Coloring with Impasse algorithm : the assigned variables respect the constraints,
an n+1 th virtual color is assigned to variables which cannot be assigned without conflicts with the
current configuration */


class ImpasseColor : public ColorCSProblem
{ public :
  ImpasseColor(int nvar, int nconst, int nbcolor, int degree);
/* indicateur de la mesure à minimiser : 0 (nombre de variables non affectées) ,
 1 (somme des degrés des variables non affectées) */
/** indicates the criterion to be minimized : 0 number of variables that cannot be assigned, 1 sum of degrees
of variables that cannot be assigned */
  int sumdegree;
  int config_evaluation(Configuration*  configuration);
  void fullincr_update_conflicts(FullincrCSPConfiguration* configuration,Move* move);
//  int move_evaluation(Configuration* configuration,int valeur,Move* move);
  void move_execution(Configuration* configuration, Move* move);
  //  int random_variable(Configuration* configuration);
  void random_configuration(Configuration* configuration);
  void compute_var_conflict(Configuration* configuration);	
  void init_domains(int nbvar, int s );
  void init_tabdomains(int s);
  void best_config_analysis();
  void best_config_write();
//  int compute_conflict (Configuration* configuration,int var, int val);

};
ColorCSProblem* probleme_coloriage(int nbvar,int nbconst,int s,int** constraint1);
ImpasseColor* coloriage_impasse(int nbvar,int nbconst,int s,int degree, int** constraint1);

ColorCSProblem* color_problem_creation (int nbcol,ifstream & file);
ImpasseColor* impasse_problem_creation (int nbcol,int degre,ifstream & file);

void lire_debut_fichier_coloriage(ifstream & file, int& nbvar, int& nbconst);
void lire_fichier_coloriage (ifstream& file, vector<int>* connexions,int ** constraint1);
int colorcsp (int argc, char** argv, int tuningmode) ;