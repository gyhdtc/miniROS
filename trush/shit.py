from mpl_toolkits.mplot3d import Axes3D 
import matplotlib.pyplot as plt 
import numpy as np 
  
c = 300000000
shit = 1000000000

x = []
y = []
z = []
for i in range(111):
    x.append(i*10+450)
for j in range(11):
    y.append(j*0.1-0.5)
for i in x:
    for j in y:
        z.append((c/(i+j)-c/i)*shit)

fig = plt.figure() 
ax = fig.gca(projection='3d') 
  
ax.plot_trisurf(x*11, y*111, z, linewidth=1, antialiased=True) 
  
plt.show()