#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>

using namespace std;
#include <fstream>
#include <string>

#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "latinsquare.h"


#include <math.h>
#include <unistd.h>



extern ofstream* ofile;  // le fichier de sortie
#define sb true



int main (int argc, char** argv) {
  int automatic_tuningmode = 1;
   if (sb) // probleme carré latin  équilibré
    latinsquare(argc,argv,automatic_tuningmode, sb);
  else  // probleme carré latin de base
    latinsquare(argc,argv,automatic_tuningmode, sb);
  return 0;
}

