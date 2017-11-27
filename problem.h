#ifndef Problem_H
#define Problem_H

#include <string>

// Defines a specific problem instance. Number of nets, terminals of net and solution?
class Problem {

public:
  struct Net {
    Net (int src, int snk) : m_srcVert (src), m_snkVert (snk) {}
    int m_srcVert, m_snkVert;
    std::vector <int> m_pathEges;
  };

  Problem () {}
  ~Problem () {}

  void addNet (int src, int snk) {
    m_nets. push_back (Net (src, snk));
  }

  int numNets () { return m_nets. size (); }
  // for SatForm to formulate the problem and deposit the paths
  Net& netRef (int netId) { return m_nets.at (netId); }

  const Net& getNet (int netId) { return m_nets.at (netId); }

  std::string getNetName (int netId) { return "Net_" + std::to_string (netId); }

private:
  std::vector <Net> m_nets;
};

#endif
