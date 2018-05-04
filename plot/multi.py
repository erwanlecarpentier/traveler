#from matplotlib.font_manager import FontProperties
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
#import sys

################################################################################

def plot_error_bar(ax, rng, mean, stddev, cl, fill):
	l = ax.errorbar(rng, mean, yerr=stddev, color=cl)
	if fill:
		v = np.copy(stddev)
		up = np.add(mean, +v)
		dw = np.add(mean, -v)
		ax.fill_between(rng, up, dw, color=cl, alpha=0.05)
	return l

def extract(path):
	data = pd.read_csv(path,sep = ',')
	t = data["elapsed_time"]
	r = data["total_return"]
	return [t, r]

def path(i,j):
	return 'data/backup/exp180412_2/svg/svg' + str(i) + '_' + str(j) + '.csv'

def plot(i,mid,ax1,ax2):
	t_mean = []
	t_std = []
	r_mean = []
	r_std = []
	for j in mid:
		[t, r] = extract(path(i,j))
		t_mean.append(t.mean())
		t_std.append(t.std())
		r_mean.append(r.mean())
		r_std.append(r.std())
	l = plot_error_bar(ax1,mid,t_mean,t_std,colors[i],True)
	plot_error_bar(ax2,mid,r_mean,r_std,colors[i],True)
	return l

################################################################################

mid = list(range(100))
colors = ['gold','darkblue','deepskyblue','coral','red']

f, (ax1,ax2) = plt.subplots(2, 1, sharex=True)

#l0 = plot(0,mid,ax1,ax2)
l1 = plot(1,mid,ax1,ax2)
l2 = plot(2,mid,ax1,ax2)
l3 = plot(3,mid,ax1,ax2)
l4 = plot(4,mid,ax1,ax2)

ax1.set_ylabel("Time Elapsed")
ax2.set_ylabel("Total Return")
ax2.set_xlabel("Number of Map")

ax1.legend([l1,l2,l3,l4], ['Dynamic UCT','Static UCT','Static Linear TMP UCT','Static Quadratic TMP UCT'])

plt.show()

