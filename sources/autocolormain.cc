


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
#include "incoputil.h"
#include "csproblem.h"
#include "color.h"


#include <math.h>
#include <unistd.h>





int main (int argc, char** argv) {
  int automatic_tuningmode = 1;
  colorcsp(argc,argv,automatic_tuningmode);
  return 0;}


