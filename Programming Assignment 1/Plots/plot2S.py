import numpy as np 
import matplotlib.pyplot as plt

t=np.array([2,4,8,16])
print (t.shape)

t1e2=np.array([534,696,694,837])
t1e2=419.0/t1e2

t1e6=np.array([141,141,135,160])
t1e6=107.0/t1e6

t1e7=np.array([110,100,113,110])
t1e7=97.0/t1e7

t1e8=np.array([104,102,99,112])
t1e8=92.0 /t1e8

plt.figure(1)
fig, ax = plt.subplots()
plt.xlabel("Processors")
plt.ylabel("Speedup")
plt.title("Speedup: Input Size Changing(Legend)")
ax.plot(t,t1e2,'y--',label='1e2 X 70')
ax.plot(t,t1e6,'r--',label='1e3 X 70')
ax.plot(t,t1e7,'g--',label='1e4 X 70')
ax.plot(t,t1e8,'b--',label='1e5 X 70')
# ax.plot(t,t2e8,'m--',label='2e8')
legend = ax.legend(loc='upper right',fontsize='x-small')
legend.get_frame().set_facecolor('#00FFCC')
plt.savefig("Part2S"+'.png')

# print t1e6
