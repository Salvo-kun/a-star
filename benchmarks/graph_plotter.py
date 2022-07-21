# -*- coding: utf-8 -*-
"""
Created on Thu Jul 21 18:57:02 2022

@author: Salvo
"""
import matplotlib.pyplot as plt
import matplotlib.colors as cols
import numpy as np
import networkx as nx
import sys

if __name__ == '__main__':
    lines =[]
    
    # lines to 2d array
    with open(sys.argv[1]) as f:
        for i in range(4):
            print(f.readline())
            
        for i in f.readlines():
            if len(i.strip()) != 0:
                line = list(map(lambda c: 0 if c == '.' else 1, i.strip()))
                lines.append(line)
        a = np.array(lines)
        
    G = nx.grid_2d_graph(*a.shape)
    
    # remove those nodes where the corresponding value is != 0
    for val,node in zip(a.ravel(), sorted(G.nodes())):
        if val!=0:
            G.remove_node(node)
            
    a[3, 5:15] = 2
    
    cMap = cols.ListedColormap(['w', 'silver', 'r'])
    plt.figure()
    plt.pcolormesh(a, cmap=cMap)
    plt.gca().set_aspect('equal') #set the x and y axes to the same scale
    plt.xticks([]) # remove the tick marks by setting to an empty list
    plt.yticks([]) # remove the tick marks by setting to an empty list
    plt.gca().invert_yaxis() #invert the y-axis so the first row of data is at the top
    plt.savefig(sys.argv[2])
    plt.show()