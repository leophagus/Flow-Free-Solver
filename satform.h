#ifndef SatForm_H
#define SatForm_H

// minisat+
#include "Global.h"
#include "Main.h"
#include "MiniSat.h"
#include "PbSolver.h"


class SatForm {
public:
  SatForm (const Graph& g, Problem& p, PbSolver& s, int dbg)
    : m_G (g), m_problem (p), m_solver (s), m_edgeVarOffset (0), m_dbg (dbg)
  {}
  ~SatForm () {}

  void formulate ();
  bool solve ();
  void readSolution ();

private:
  void buildVars ();
  void addContinuity ();
  void addVertex (int netId, int nodeId, bool term);
  void addNodeCapacity ();
  void addEdgeCapacity ();
  void addObj ();

  int getEdgeVar (int netId, int edgeId) { 
    int idx = netId * m_G.numEdges () + edgeId;
    return m_edgeVars. at (idx); 
  }
  int getNodeVar (int netId, int nodeId) { 
    int idx = netId * m_G.numNodes () + nodeId;
    return m_nodeVars. at (idx); 
  }

  void printConstr (const vec<Lit>& ps, const vec<Int>& cs, Int rhs, int ineq);

private:
  const Graph& m_G;
  Problem&     m_problem;
  PbSolver&    m_solver;

  int          m_dbg;
  int          m_edgeVarOffset;
  std::vector <int> m_nodeVars, m_edgeVars;
};

#endif
