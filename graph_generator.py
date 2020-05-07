#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Nov  5 11:32:27 2019

@author: mdaa
"""

import matplotlib.pyplot as plt
#import numpy as np

import pandas as pd

#data = pd.read_csv('/home/mdaa/eclipse-workspace/slice_resource/user_alloc_20_slice.csv', sep=';', index_col = False)
data = pd.read_csv('/home/mdaa/eclipse-workspace/slice_resource/user_alloc.csv', sep=';', index_col = False)


x1_prio = []
x1_prio_old = []
x1_req = []
x1_alloc = []
x1_pre_alloc = []
y = []


#For 2nd user
x1_prio2 = []
x1_prio2_old = []
x1_req2 = []
x1_alloc2 = []
x1_pre_alloc2 = []

#slice_id = data.iloc[:,0:1].values
#prio = data.iloc[:,4:5].values
#req = data.iloc[:,2:3].values
#alloc = data.iloc[:,5:6].values

slice_id = data.iloc[:,1:2].values
prio = data.iloc[:,5:6].values
prio_old = data.iloc[:,2:3].values
req = data.iloc[:,3:4].values
pre_alloc = data.iloc[:,4:5].values
alloc = data.iloc[:,6:7].values


#For 2nd user
prio2 = data.iloc[:,5:6].values
req2 = data.iloc[:,3:4].values
pre_alloc2 = data.iloc[:,4:5].values
alloc2 = data.iloc[:,6:7].values

y_scale = 0
for index in range (data.shape[0]):
#    if slice_id[index,0] == 3:
     if slice_id[index,0] == 18:
        x1_prio.append (prio[index,0])
        x1_prio_old.append (prio_old[index,0])
        x1_req.append (req[index,0])
        x1_alloc.append (alloc[index,0])
        x1_pre_alloc.append (pre_alloc[index,0])
#        y.append(y_scale)
#        y_scale = y_scale + 1
        
        
        
plt.plot(x1_prio, color = 'green', label = 'Slice Priority', marker='d', markersize=3)
plt.ylim (0,100)
plt.xlabel('Time (s)')
plt.ylabel('Priority')
plt.grid(which='major', linestyle='-', linewidth='0.5', color='gray')
plt.legend(loc='best', prop={'size': 6})
plt.savefig('slice_prio.png',bbox_inches="tight", pad_inches=0, dpi=300)
plt.show()


plt.plot(x1_req, color = 'brown', label = 'Predicted Data Rate', linestyle = '--', marker='x', markersize=2)
plt.plot(x1_pre_alloc, color = 'red', label = 'Allocated Resource Before Algorithm', marker='^', markersize=2)
plt.plot(x1_alloc, color = 'green', label = 'Allocated Resource After Algorithm', marker='o', markersize=2)
#plt.ylim (0,200)
#plt.title('Traffic Prediction')
plt.xlabel('Time (s)')
plt.ylabel('Data Rate (MBps)')
plt.grid(which='major', linestyle='-', linewidth='0.5', color='gray')
plt.legend(loc='best', prop={'size': 6})
plt.savefig('res_alloc.png',bbox_inches="tight", pad_inches=0.1, dpi=300)
plt.show()



#2 slice comperison
for index in range (data.shape[0]):
#    if slice_id[index,0] == 3:
     if slice_id[index,0] == 14:
        x1_prio2.append (prio[index,0])
        x1_prio2_old.append (prio_old[index,0])
        x1_req2.append (req[index,0])
        x1_alloc2.append (alloc[index,0])
        x1_pre_alloc2.append (pre_alloc[index,0])
        


fig = plt.figure()
ax = fig.add_subplot(111)

lns1 = ax.plot(x1_req, color = 'brown', label = 'Slice 1 request', linestyle = '--', marker='x', markersize=2)
lns2 = ax.plot(x1_alloc, color = 'green', label = 'Slice 1 Allocation', marker='o', markersize=2)
lns4 = ax.plot(x1_req2, color = 'purple', label = 'Slice 2 request', linestyle = '--', marker='^', markersize=2)
lns5 = ax.plot(x1_alloc2, color = 'orange', label = 'Slice 2 allocation', marker='+', markersize=2)

ax2 = ax.twinx()
lns3 = ax2.plot(x1_prio, color = 'blue', label = 'Slice 1 Priority', marker='d', markersize=3)
lns6 = ax2.plot(x1_prio2, color = 'red', label = 'Slice 2 Priority', marker='*', markersize=3)


# added these six lines
lns = lns1+lns2+lns3+lns4+lns5+lns6
labs = [l.get_label() for l in lns]
ax.legend(lns, labs, loc='best', prop={'size': 6})

ax.grid()
ax.set_xlabel("Time (s)")
ax.set_ylabel(r"Data Rate (MBps)")
ax2.set_ylabel(r"Priority")
ax2.set_ylim(0, 130)
ax.set_ylim(0,70)
plt.savefig('slice_comp.png',bbox_inches="tight", pad_inches=0.1, dpi=300)
plt.show()




res_req =  []
res_alloc = []
res_available = []
res_preAloc = []
total_slice = 20
total_req = 0
total_resource = 0
resource_preAlgorithm = 0

#for index in range (data.shape[0]):
#    if (((index % total_slice) > 0) or (index == 0)):
#        total_req = total_req + req[index,0]
#    else:
#        req_arr.append (total_req)
#        total_res.append (total_resource)
#        total_req = 0

total_slice = 20    
slice_count = 0
simulation_time = 60
total_res = 0
#Check this valiue from the c code named "#define TOTAL_CAPACITY"
const_res = 560


for index in range (total_slice):
    total_res = total_res + alloc[(index),0]

for time in range (simulation_time):
    for index in range (total_slice):
        total_req = total_req + req[(slice_count+index),0]
#        print (total_req)
        total_resource = total_resource + alloc[(slice_count+index),0]
        resource_preAlgorithm = resource_preAlgorithm + pre_alloc[(slice_count+index),0]

    slice_count = slice_count + total_slice
    res_req.append (total_req)
    res_alloc.append (total_resource)
#    res_available.append (total_res)
    res_available.append (const_res)
    res_preAloc.append (resource_preAlgorithm)
    
    total_req = 0
    total_resource = 0
    resource_preAlgorithm = 0
    
plt.plot(res_req, color = 'orange', label = 'Slice Request', marker='d', markersize=3)
plt.plot(res_preAloc, color = 'blue', label = 'Resource Allocation Before Algorithm', ls= ':',marker='^', markersize=2)
plt.plot(res_alloc, color = 'brown', label = 'Resource Allocation After Algorithm', ls= ':',marker='p', markersize=2)
plt.plot(res_available, color = 'green', label = 'Maximum Resource Available', ls= ':',marker='+', markersize=2)
plt.grid(which='major', linestyle='-', linewidth='0.5', color='gray')
#plt.ylim (3500,9000)
plt.xlabel('Time (s)')
plt.ylabel('Data Rate (MBps)')
plt.legend(loc='best', prop={'size': 6})
plt.savefig('total_req.png',bbox_inches="tight", pad_inches=0.1, dpi=300)
plt.show()