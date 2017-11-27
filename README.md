# Flow-Free-Solver
SAT based solver for popular mobile game, *Flow Free*. The game makes you find connected disjoint paths for multiple networks on a 2-D grid. This is an exact solver and *will* find a solution if one exists. Models the problem as a pseudo-boolean SAT formulation and uses [MiniSAT+](http://minisat.se/MiniSat+.html) to solve.

## Install
Download MiniSAT+ from the URL above and build it. Modify the Makefile to point to your MiniSAT+ build and do make. 

## TODO
1. The game description (grid size, locations of the network endpoints) are read interactively or from a file. 
2. The solution is output in plain-text. A curses GUI would be nice.
3. Integrate into a mobile App or a web service maybe ?
