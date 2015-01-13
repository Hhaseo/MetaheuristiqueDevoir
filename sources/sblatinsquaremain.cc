#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>

using namespace std;
#include <fstream.h>
#include <string>

#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "latinsquare.h"


#include <math.h>
#include <unistd.h>




int main (int argc, char** argv) {
  int automatic_tuningmode = 0;
  latinsquare(argc,argv,automatic_tuningmode,1);
  return 0;
}

