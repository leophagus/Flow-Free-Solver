#include <iostream>
#include <vector>

#include "board.h"
#include "problem.h"
#include "satform.h"

void SatForm::buildVars ()
{
  std::cout << "buildVars start..." << std::endl;

  for (int n=0; n < m_problem.numNets (); ++n) {
    for (int v=0; v < m_G.numNodes (); ++v) {
      std::string nvName = "n" + std::to_string (n) + "x" + std::to_string (v);
      int nvInd = m_solver. getVar (nvName. c_str ());
      m_nodeVars. push_back (nvInd);
    }
  }

  m_edgeVarOffset = m_nodeVars. size ();
  for (int n=0; n < m_problem.numNets (); ++n) {
    for (int e=0; e < m_G.numEdges (); ++e) {
      const auto& edge = m_G.getEdge (e);
      std::string neName = "n" + std::to_string (n) + "y" + 
        std::to_string (edge.getI()) + std::to_string (edge.getJ());
      int neInd = m_solver. getVar (neName. c_str ());
      m_edgeVars. push_back (neInd);
    }
  }
}

void SatForm::formulate ()
{
  buildVars ();

  addObj ();

  addContinuity ();

  addNodeCapacity ();

  addEdgeCapacity ();
}

bool SatForm::solve ()
{
  std::cout << "Model stats: " << m_solver. nVars () << " vars, " 
            << m_solver. nConstrs () << " constrs " << std::endl;
  m_solver. solve (PbSolver::sc_Minimize);
  if (m_solver. best_goalvalue == Int_MAX) {
    std::cout << "Unsatisfiable" << std::endl;
    return false;
  } else {
    std::cout << "Optimum found:" << std::endl;
    //for (int i=0; i < m_solver. best_model. size (); ++i) {
    //  std::cout << (m_solver. best_model [i]?"":"-") << m_solver. index2name[i] << " ";
    //}
    //std::cout << std::endl;
    return true;
  }
}

void SatForm::readSolution ()
{
  for (int netId=0; netId < m_problem. numNets (); ++netId) {
    std::cout << "Net " << netId << ": ";
    for (int nodeId=0; nodeId < m_G. numNodes (); ++nodeId) {
      int idx = netId * m_G.numNodes () + nodeId;
      if (m_solver. best_model [idx]) 
        std::cout << m_solver. index2name[idx] << " ";
    }
    std::cout << std::endl;
  }
}

void SatForm::addContinuity ()
{
  for (int netId=0; netId < m_problem.numNets (); ++netId) {
    auto& net = m_problem. getNet (netId);
    int src = net. m_srcVert;
    int snk = net. m_snkVert;
    for (int nodeId=0; nodeId < m_G. numNodes (); ++ nodeId) {
      bool bTerm = nodeId == src || nodeId == snk;
      addVertex (netId, nodeId, bTerm);
    }
  }
}

// term:
//    1*x = 1
//    \sigma 1*x_edges = 1
// non-term
//    -2*x + \sigma 1*x_edges = 0
void SatForm::addVertex (int netId, int nodeId, bool term)
{
  if (term) {
    {
      vec<Int> cs;  // co-effs
      vec<Lit> ps;  // variable
      cs. push ( Int (1));
      int nodeVar = getNodeVar (netId, nodeId);
      ps. push ( Lit (nodeVar));
      Int rhs (1);
      int ineq = 0;
      m_solver. addConstr (ps, cs, rhs, ineq);
      if (m_dbg) printConstr (ps, cs, rhs, ineq);
    }
    {
      vec<Int> cs;  // co-effs
      vec<Lit> ps;  // variable
      auto& node = m_G.getNode (nodeId);
      for (auto eId : node. getEdges ()) {
        cs. push ( Int (1));
        int edgeVar = getEdgeVar (netId, eId);
        ps. push ( Lit (edgeVar));
      }

      Int rhs (1);
      int ineq = 0;
      m_solver. addConstr (ps, cs, rhs, ineq);
      if (m_dbg) printConstr (ps, cs, rhs, ineq);
    }

  } else { 

    vec<Int> cs;  // co-effs
    vec<Lit> ps;  // variable
    cs. push ( Int (-2));
    int nodeVar = getNodeVar (netId, nodeId);
    ps. push ( Lit (nodeVar));

    auto& node = m_G.getNode (nodeId);
    for (auto eId : node. getEdges ()) {
      cs. push ( Int (1));
      int edgeVar = getEdgeVar (netId, eId);
      ps. push ( Lit (edgeVar));
    }

    Int rhs (0);
    int ineq = 0;
    m_solver. addConstr (ps, cs, rhs, ineq);
    if (m_dbg) printConstr (ps, cs, rhs, ineq);
  }
}

// Global capacity constraint
//    forall vertices, \sigma_net vertex <=1
// +1*n0x0 +1*n1x0 <= 1;
void SatForm::addNodeCapacity ()
{
  for (int nodeId=0; nodeId < m_G. numNodes (); ++nodeId) {
    vec<Int> cs;  // co-effs
    vec<Lit> ps;  // variable
    for (int netId=0; netId < m_problem.numNets (); ++netId) {
      int nodeVar = getNodeVar (netId, nodeId);
      cs. push ( Int (1));
      ps. push ( Lit (nodeVar));
    }
    Int rhs (1);
    int ineq = -1; // <=
    m_solver. addConstr (ps, cs, rhs, ineq);

    if (m_dbg) printConstr (ps, cs, rhs, ineq);
  }
}

void SatForm::printConstr (const vec<Lit>& ps, const vec<Int>& cs, Int rhs, int ineq)
{
  for (int i=0; i < cs.size (); ++i) {
    int csi = toint (cs [i]);
    int psi = var (ps[i]);
    std::cout << (csi >=0 ?"+":"") << csi << "*" << m_solver. index2name [psi] << " "; 
  }
  switch (ineq) {
  case -2: std::cout << " < "; break;
  case -1: std::cout << " <= "; break;
  case  0: std::cout << " == "; break;
  case  1: std::cout << " >= "; break;
  case  2: std::cout << " > "; break;
  default: std::cout << " ?? ";
  }
  std::cout << toint (rhs) << std::endl;
}

// Global capacity constraint
//    forall edges, \sigma_net edge <= 1
// +1*n0y0 +1*n1y0 <= 1; TODO do I need this?
void SatForm::addEdgeCapacity ()
{
  for (int eId=0; eId < m_G.numEdges (); ++eId) {
    vec<Int> cs;  // co-effs
    vec<Lit> ps;  // variable
    for (int netId=0; netId < m_problem.numNets (); ++netId) {
      int edgeVar = getEdgeVar (netId, eId);
      cs. push ( Int (1));
      ps. push ( Lit (edgeVar));
    }
    Int rhs (1);
    int ineq = -1; // <=
    m_solver. addConstr (ps, cs, rhs, ineq);

    if (m_dbg) printConstr (ps, cs, rhs, ineq);
  }
}

// min: \sigma edgeVars (min total wirelength)
void SatForm::addObj ()
{
  vec<Int> goal_cs;  // co-effs
  vec<Lit> goal_ps;  // variable
  for (int netId=0; netId < m_problem.numNets (); ++netId) {
    for (int eId=0; eId < m_G.numEdges (); ++eId) {
      int edgeVar = getEdgeVar (netId, eId);
      goal_cs. push ( Int (1));
      goal_ps. push ( Lit (edgeVar));
    }
  }
  m_solver. addGoal (goal_ps, goal_cs);
}

