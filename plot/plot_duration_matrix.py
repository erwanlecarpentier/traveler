import matplotlib.pyplot as plt
from matplotlib.lines import Line2D

#path = 'config/school_case.csv'
path = 'config/map0.csv'

# Extract
with open(path) as f:
    lis=[line.split() for line in f]

rows = []
for i in range(len(lis)):
	rows.append(lis[i][0].split(";"))


# Parse

rng = rows[0][2:]
h = []

plot_all = False
max_number_of_edges = 10
if plot_all:
	max_number_of_edges = len(rows)

for i in range(1,max_number_of_edges+1):
	orig = rows[i][0]
	dest = rows[i][1]
	lab = orig + ' - ' + dest
	cstrng = rows[i][2:]
	cfloat = []
	for c in cstrng:
		cfloat.append(float(c))
	h += plt.plot(cfloat, label=lab)

plt.legend(handles=h)

plt.show()

