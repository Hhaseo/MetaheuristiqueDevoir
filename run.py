#!/usr/bin/python
import os
import sys

# Made for Guillaume Desquesnes & Florian Benavent homework of Metaheuristique based on Mr. Loudni script.

NINSTANCES=1 #20
MAXTABU=1500000 #{4} -> nbmouv
NB=20 #{3}
NEXTNEIGH=1 #{8} -> fin voisinage ?
DYNNEIGH=1 #{9}
MINNEIGH=1 #{6}
TRACE=0 #{10}
TABU=20 #{5}
SEED=0
DIVE=0
TEMP=0.5
MAXNEIGH=50 #{7}
KMAX = 1 #{11}
TIME = 3600 #{12}
#for MAXNEIGH in 50 #100 150 175 #do	

os.system("make colorcsp -C sources"); 
# run.py instance nbColor
n=0 #{2}
while (n <= NINSTANCES):
	print "max_neighbours:",MAXNEIGH,":it",n	
	#~ os.system("./sources/colorcsp out/result{2}.txt {0} {1} {2} {3} idwaincrtabu {4} {5} cv mc {6} {7} {8} {9} {10}".format(sys.argv[1],sys.argv[2],n,NB,MAXTABU,TABU,MINNEIGH,MAXNEIGH,NEXTNEIGH,DYNNEIGH,TRACE))
	os.system("./sources/colorcsp out/result{2}.txt {0} {1} {2} {3} vns tabu {4} {5} cv mc {6} {7} {8} {9} {11} {12} {10}".format(sys.argv[1],sys.argv[2],n,NB,
			MAXTABU,TABU,MINNEIGH,MAXNEIGH,NEXTNEIGH,DYNNEIGH,TRACE,KMAX,TIME))
	print
	n += 1
