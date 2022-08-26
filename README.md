
# A-Star 
A* is a C program providing multiple implementations of the famous pathfinding algorithm. 
The available versions are:
- Sequential (standard implementation of the A* algorithm)
- Parallel (decentralized implementation of the A* algorithm using threads)
- Dijkstra (implementated as the sequential A* without heuristic)

## Table of Contents

- [About](#about)
- [Getting Started](#getting_started)
- [Usage](#usage)
- [Graph generation](#graph-generation)
- [Drawing path](#drawing-path)
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


## Usage
The program can be built using `make` command from a linux shell.  
To run the program use `./bin/aStar [OPTION...] IN_FILE`

To choose the algorithm use `-a ALGORITHM` where ALGORITHM can be:
- 0 for Dijkstra
- 1 for sequential A*
- 2 for parallel A*

To choose the domain use `-d DOMAIN` where DOMAIN can be:
- 0 for 2D grids

To choose the heuristic use `-e HEURISTIC` where HEURISTIC can be:
- 0 for Manhattan distance for 2D grids

To choose hash function of parallel A* use `-h HASH` where HASH can be:
- 0 for Module hash
- 1 for Multiplicative hash (suggested)

To choose the output file use `-o OUTPUT_FILE`, if not specified the default is stdout

To choose the number of threads used by the parallel A* use `-t NUMBER_OF_THREADS`

Run `./bin/aStar --help` and `./bin/aStar --usage` in the root folder of the project to get additional info about it.

_Example of parallel A* usage_:
`./bin/aStar -a 2 -h 1 -t 4 -o ./output_file.txt ./input_file.txt`

-----------------------------------

## Graph generation

The project includes a python3 script to generate the proper files needed by the A-Star program.
The script translate a .map file in a .txt ones with random weights for the edges.

#### Usage

To generate the graph use the command `python3 ./benchmarks/graph_generator.py IN_FILE.map  OUT_FILE.txt  GRID_DIMENSION  MAX_WEIGHT`

_Example of graph generation usage_:
`python3 ./benchmarks/graph_generator.py ./benchmarks/street-maps/raw/Milan_0_1024.map ./output_file.txt 1024 100`

------------------------------------------

## Drawing path

The project includes a python3 script to draw the path found by the A-Star program inside a map.

#### Usage

To plot the path use the command `python3 ./benchmarks/graph_plotter.py IN_FILE.map  OUT_FILE.png  GRID_DIMENSION ./path_found.txt`

_Example of path plot usage_:
`python3 ./benchmarks/graph_plotter.py ./benchmarks/street-maps/raw/Milan_0_1024.map  ./output_file.png  1024 ./path_found.txt`


## Contributing

Salvatore Licata,
Alessandro Zamparutti,
Lorenzo Ferro