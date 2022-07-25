# -*- coding: utf-8 -*-
"""
Created on Mon Jul 18 18:33:18 2022

@author: Salvo
"""

import numpy as np
import networkx as nx
import sys

if __name__ == '__main__':
    lines = []
    size = int(sys.argv[3])
    
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
    
    # generate graph, first nodes with data, then edges
    with open(sys.argv[2], 'w') as file:
        nodes = list(G.nodes())
        edges = list(G.edges())
        directed = 1 if nx.is_directed(G) else 0
        
        file.write(str(len(nodes)) + ' ' + str(directed) + '\n')
        
        for node in nodes:
            x, y = node
            file.write(str(size*x+y) + ' ' + str(x) + ' ' + str(y) + '\n')
            
        for edge in edges:
            x1, y1 = edge[0]
            x2, y2 = edge[1]
            file.write(str(size*x1+y1) + ' ' +  str(size*x2+y2) + ' 1\n')
    