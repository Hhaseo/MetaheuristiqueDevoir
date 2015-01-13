#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
using namespace std;
#include <iostream>
#include <fstream>
#include "timer.h"
#include "incop.h"
#include "csproblem.h"
#include "incoputil.h"

#include "narycsproblem.h"
#include "autotuning2.h"


#include <math.h>
#include <unistd.h>

/** le lecteur des fichiers au format wcsp de Simon de Givry
    pour le moment, ne lit que des problèmes avec contraintes  en extension , les valeurs des domaines
doivent être des entiers.
*/


int main (int argc, char** argv) {
  int automatic_tuningmode = 1;
  narycsp(argc,argv,automatic_tuningmode);
  return 0;}





