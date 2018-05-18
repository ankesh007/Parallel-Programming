import numpy as np 
import matplotlib.pyplot as plt

t=np.array([2,4,8,16])
print (t.shape)

t1e5=np.array([0.0027,0.0022,0.0028,0.0028])
t1e5=0.003/t1e5

t1e6=np.array([0.01,0.014,0.018,0.018])
t1e6=0.012/t1e6

t1e7=np.array([0.12,0.105,0.125,0.12])
t1e7=0.125/t1e7

t1e8=np.array([0.99,0.78,0.99,1])
t1e8=1.15/t1e8

t2e8=np.array([1.95,1.52,1.94,1.96])
t2e8=2.24/t2e8

plt.figure(1)
fig, ax = plt.subplots()
plt.xlabel("Processors")
plt.ylabel("Speedup")
plt.title("Speedup: Input Size Changing(Legend)")
ax.plot(t,t1e5,'y--',label='1e5')
ax.plot(t,t1e6,'r--',label='1e6')
ax.plot(t,t1e7,'g--',label='1e7')
ax.plot(t,t1e8,'b--',label='1e8')
ax.plot(t,t2e8,'m--',label='2e8')
legend = ax.legend(loc='upper center',fontsize='x-small')
legend.get_frame().set_facecolor('#00FFCC')
plt.savefig("Part1S"+'.png')

# print t1e6
