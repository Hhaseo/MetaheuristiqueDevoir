import glob
from collections import defaultdict


outputs = glob.glob("out/*.txt")
conflicts = defaultdict(float)
duration = defaultdict(float)

for s in outputs:
	with open(s,'r') as f:
		lines = f.readlines()
		graph_id = s[4:-6]+"+COLOR"+s[-5]
		conflicts[graph_id] += 0.5 * float(lines[-6].split()[-1])
		duration[graph_id]  += 0.5 * float(lines[-4].split()[-1])

with open("conflicts.txt",'w') as f:
	for k in conflicts:
		f.write(k+"->"+str(conflicts[k])+"\n")
with open("duration.txt","w") as f:
	for k in duration:
		f.write(k+"->"+str(duration[k])+"\n")
