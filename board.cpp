#include <iostream>
#include "board.h"

// builds a 2D mesh graph
void Board::build ()
{
  // add vertices
  int nNodes = m_rows * m_cols;
  for (int i=0; i < nNodes; ++i) {
    m_G. addNode ();
  }

  int offsets[][2] = { {1,0}, {0,1} };

  // add bi-dir edges between horizontally and vertically adjacent vertices
  for (int r=0; r<m_rows; ++r) {
    for (int c=0; c<m_cols; ++c) {
      int idxI = r*m_rows + c;

      for (int off=0; off < 2; ++off) {
        int rr = r + offsets [off][0];
        int cc = c + offsets [off][1];

        if (rr <0 || rr >= m_rows) continue;
        if (cc <0 || cc >= m_cols) continue;

        int idxJ = rr*m_rows + cc;
        m_G.addEdge (idxI, idxJ);

      }

    }
  }

  //m_G. printGraph ();
}

int Board::cinVertex () const
{
  while (1) {
    int r,c;
    std::cout << "Vertex row [0," << m_rows << ")? "; std::cin >> r;
    std::cout << "Vertex col [0," << m_cols << ")? "; std::cin >> c;
    if (r<0 || r>= m_rows || c<0 || c>=m_cols) {
      std::cout << "Invalid entry." << std::endl;
    } else {
      return r*m_rows + c;
    }
  }
}

int Board::cinEdge () const
{
  while (1) {
    std::cout << "Edge?" << std::endl;
    int nI = cinVertex ();
    int nJ = cinVertex ();
    int eId = m_G.getEdge (nI, nJ);
    if (eId != m_G.BAD_ID) {
      return eId;
    } else {
      std::cout << "Edge not found." << std::endl;
    }
  }
}

void Graph::printGraph ()
{
  for (const auto& node : m_nodes) {
    std::cout << "Node " << node.id() << " Edges: ";
    for (const auto eId : node. getEdges ()) 
      std::cout << eId << " ";
    std::cout << std::endl;
  }

  int eId = 0;
  for (const auto& edge : m_edges) {
    std::cout << "Edge " << eId++ << " " << edge.getI() << " -> " << edge.getJ () << " "
      << edge. getNodeI ().id () << " -> " << edge. getNodeJ ().id () << std::endl;
  }
  std::cout << "printGraph done" << std::endl;
}

