
# A-Star 
A* is a C program providing multiple implementations of the famous pathfinding algorithm. 
The available versions are:
- Sequential (standard implementation of the A* algorithm)
- Parallel (decentralized implementation of the A* algorithm using threads)
- Dijkstra (implementated as the sequential A* without heuristic)

## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Installing](#installing)
- [Usage](#usage)
- [Contributing](#contributing)

----------------------------------------------------------

## About
The program can be used to find the optimal path (in terms of cost) from a source to a destination node on large graphs.

## Getting Started
The program MUST be run on a Linux environment, since it uses the POSIX thread (pthread) libraries.
The program receives as input `.txt` files with the following format:
- first line with the total number of nodes and the type of the graph (0 if undirected, 1 if directed)
- One line for each node with variable number of fields: node id, additional data (eg. x,y coordinates for 2D grid)
- Variable number of lines with two fields: start node id, end node id (edge)

A set of different benchmarks are provided inside the project (`.map` files).

## Installing


## Usage
The program can be built using `make` command from a linux shell.  
To run the program use `.\bin\aStar [OPTION...] IN_FILE`

To choose the algorithm use `-a ALGORITHM` where ALGORITHM can be:
- 0 for Dijkstra
- 1 for sequential A*
- 2 for parallel A*

To choose the domain use `-d DOMAIN` where DOMAIN can be:
- 0 for 2D grids

To choose the heuristic use `-e HEURISTIC` where HEURISTIC can be:
- 0 for Manhattan distance for 2D grids

To choose hash function of parallel A* use `-h HASH` where HASH ca be:
- 0 forModule hash
- 1 for Multiplicative hash

To choose the output file use `-o OUTPUT_FILE`

To choose the number of threads used by the parallel A* use `-t NUMBER_OF_THREADS`

Run `./bin/aStar --help` and `.\bin\aStar --usage` in the root folder of the project to get additional info about it.

_Example of parallel A* usage_:
`.\bin\aStar -a 2 -h 1 -t 4 -o .\output_file.txt .\input_file.txt`


## Contributing

Salvatore Licata
Alessandro Zamparutti
Lorenzo Ferro