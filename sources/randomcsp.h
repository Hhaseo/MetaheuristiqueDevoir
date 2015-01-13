ExtensionBinaryCSProblem* probleme_randomcsp(int nbvar, int nbconst, int s, int ** constraint1, int*** constraint2, int t, int graine,int p2);
ExtensionBinaryCSProblem* random_problem_creation (int nbvar, int nbconst, int s, int ** constraint1, int*** constraint2, int t, int graine,int p2, vector<int>* connexions);
void arguments_csp_alea (char** argv, int& narg, int& nbvar, int& s, int & nbconst, int& p2, 
int & graine, int & npb);

int randomcsp(int argc,char** argv,int automatic_tuningmode);