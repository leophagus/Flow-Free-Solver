#ifndef __BOARD_H_
#define __BOARD_H_
#include <vector>

class Graph {
public:
  class Node {
    public:
      Node (int id) : m_id (id) {}
      int id () const { return m_id; }
      void addEdge (int edgeId) {
        m_edges. push_back (edgeId);
      }
      const std::vector <int>& getEdges () const { return m_edges; }
    private:
      int m_id;
      std::vector <int> m_edges;
  };

  class Edge {
    public:
      Edge (int i, int j, const Node& ni, const Node& nj) 
        : m_i (i), m_j (j), m_ni (ni), m_nj (nj) {}
      int getI () const { return m_i; }
      int getJ () const { return m_j; }
      const Node& getNodeI () const { return m_ni; }
      const Node& getNodeJ () const { return m_nj; }
      bool isIncident (int nId) const { return m_i == nId || m_j == nId; }
      bool isIncident (const Node& node) const { return &m_ni == &node || &m_nj == &node; }
    private:
      int m_i, m_j;
      const Node& m_ni;
      const Node& m_nj;
  };

  Graph () {}
  ~Graph () {}

  int addNode () { 
    int nodeId = m_nodes.size ();
    m_nodes. push_back (Node (nodeId));
    return nodeId; 
  }
  int addEdge (int i, int j) {
    auto& ni = nodeRef (i);
    auto& nj = nodeRef (j);
    int edgeId = m_edges.size ();
    m_edges.push_back (Edge(i, j, ni, nj) );
    // edges are bi-dir
    ni.addEdge (edgeId);
    nj.addEdge (edgeId);
    return edgeId;
  }

  int numNodes () const { return m_nodes. size (); }
  int numEdges () const { return m_edges. size (); }

  const Node& getNode (int id) const { return m_nodes.at (id); }
  const Edge& getEdge (int id) const { return m_edges.at (id); }

  Node& nodeRef (int id) { return m_nodes.at (id); }

  int getEdge (int nidI, int nidJ) const {
    const auto& nI = getNode (nidI);
    for (const auto eId : nI.getEdges ()) {
      const auto& edge = getEdge (eId);
      if (edge. isIncident (nidJ)) {
        return eId;
      }
    }
    return BAD_ID;  // not found
  }

  const int BAD_ID = -1;

  void printGraph ();

private:
  std::vector <Node> m_nodes;
  std::vector <Edge> m_edges;
};

// Board uses a Graph to model the vertices and edges. Provides methods to
// build the Graph and various accessors
class Board {
public:
  Board (int nRows, int nCols) 
    : m_rows(nRows), m_cols(nCols) 
  {
    build ();
  }
  ~Board () {}

  const Graph& getGraph () const { return m_G; }

  // use cin to get info and return vertx/edge already in G
  int cinVertex () const;
  int cinEdge () const;

protected:
  void build ();

protected:
  Graph m_G;
  int m_rows, m_cols;

};

#endif
