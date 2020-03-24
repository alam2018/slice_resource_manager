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
x1_req = []
x1_alloc = []
y = []

slice_id = data.iloc[:,0:1].values
prio = data.iloc[:,4:5].values
req = data.iloc[:,2:3].values
alloc = data.iloc[:,5:6].values


y_scale = 0
for index in range (data.shape[0]):
    if slice_id[index,0] == 3:
        x1_prio.append (prio[index,0])
        x1_req.append (req[index,0])
        x1_alloc.append (alloc[index,0])
#        y.append(y_scale)
#        y_scale = y_scale + 1
        
        
plt.plot(x1_prio, color = 'green', label = 'Slice Priority', marker='d', markersize=3)
plt.ylim (0,300)
plt.xlabel('Time Index (s)')
plt.ylabel('Priority')
plt.grid(which='major', linestyle='-', linewidth='0.05', color='gray')
plt.legend(loc='best', prop={'size': 6})
plt.savefig('slice_prio.png',bbox_inches="tight", pad_inches=0, dpi=300)
plt.show()


plt.plot(x1_req, color = 'blue', label = 'Predicted traffic request', linestyle = '--', marker='x', markersize=3)
plt.plot(x1_alloc, color = 'red', label = 'Allocated Resources', marker='o', markersize=3)
plt.ylim (0,800)
#plt.title('Traffic Prediction')
plt.xlabel('Time Index (s)')
plt.ylabel('Data (MB)')
plt.grid(which='major', linestyle='-', linewidth='0.05', color='gray')
plt.legend(loc='best', prop={'size': 6})
plt.savefig('res_alloc.png',bbox_inches="tight", pad_inches=0.1, dpi=300)
plt.show()

req_arr =  []
total_res = []
total_slice = 20
total_req = 0
total_resource = 5322

for index in range (data.shape[0]):
    if (((index % total_slice) > 0) or (index == 0)):
        total_req = total_req + req[index,0]
    else:
        req_arr.append (total_req)
        total_res.append (total_resource)
        total_req = 0
    
    
plt.plot(req_arr, color = 'orange', label = 'Slice Request', marker='d', markersize=3)
plt.plot(total_res, color = 'brown', label = 'Resource Allocation', ls= ':',marker='p', markersize=2)
plt.grid(which='major', linestyle='-', linewidth='0.05', color='gray')
plt.ylim (3500,9000)
plt.xlabel('Time Index (s)')
plt.ylabel('Total Request (MB)')
plt.legend(loc='best', prop={'size': 6})
plt.savefig('total_req.png',bbox_inches="tight", pad_inches=0.1, dpi=300)
plt.show()