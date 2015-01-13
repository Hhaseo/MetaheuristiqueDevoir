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
#include "move.h"
#include "carseq.h"


#include <math.h>
#include <unistd.h>





int main (int argc, char** argv) {
  int automatic_tuningmode = 0;
carseq(argc,argv,automatic_tuningmode);
  return 0;
}
