#!/usr/bin/python
import os
import sys

NINSTANCES=1 #20
MAXTABU=1500000
NB=20
NEXTNEIGH=1
DYNNEIGH=1
MINNEIGH=1
TRACE=0
TABU=20
SEED=0
DIVE=0
TEMP=0.5
MAXNEIGH=50 
#for MAXNEIGH in 50 #100 150 175 #do	

# run.py instance nbColor
n=0
while (n <= NINSTANCES):
	print "max_neighbours:",MAXNEIGH,":it",n	
	os.system("./sources/colorcsp arg {0} {1} {2} {3} idwaincrtabu {4} {5} cv mc {6} {7} {8} {9} {10}".format(sys.argv[1],sys.argv[2],n,NB,MAXTABU,TABU,MINNEIGH,MAXNEIGH,NEXTNEIGH,DYNNEIGH,TRACE))
	print
	n += 1
