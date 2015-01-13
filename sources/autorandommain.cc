#include <cerrno>
#include <stdio.h>
#include <list>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
using namespace std;
#include <iostream>
#include <fstream>
#include "timer.h"
#include "incop.h"
#include "csproblem.h"
#include "incoputil.h"
#include "urbcsp2.h"
#include "extcsp.h"
#include "autotuning2.h"
#include "randomcsp.h"

#include <math.h>
#include <unistd.h>




int main (int argc, char** argv) {
  int automatic_tuningmode = 1;
  randomcsp(argc,argv,automatic_tuningmode);
  return 0;}
