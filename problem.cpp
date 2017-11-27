#include <iostream>
#include <vector>
#include "board.h"
#include "problem.h"

void buildProblem (const Board& B, Problem& P)
{
  int nNets=0;
  std::cout << "Number of Nets? "; std::cin >> nNets;
  for (int i=0; i<nNets; ++i) {
    std::cout << "Net " << i << " vertices?" << std::endl;
    int nidI = B.cinVertex ();
    int nidJ = B.cinVertex ();
    P.addNet (nidI, nidJ);
  }
}

