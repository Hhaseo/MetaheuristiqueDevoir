
#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <fstream.h>
#include "timer.h"
#include "incop.h"
#include "incoputil.h"
#include "csproblem.h"
#include "color.h"
#include "maxclique.h"
#include "autotuning2.h"
   


#include <math.h>
#include <unistd.h>


int main (int argc, char** argv) {
  int automatic_tuningmode = 1;
  maxclique (argc,argv,automatic_tuningmode);
  return 0;}








