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
#include "autotuning2.h"

#include <math.h>
#include <unistd.h>



int main (int argc, char** argv) {
  int automatic_tuningmode = 1;
  latinsquare(argc,argv,automatic_tuningmode,1);
  return 0;
}

