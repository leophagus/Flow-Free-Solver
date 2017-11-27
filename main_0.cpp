#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstdarg>
#include <map>
// minisat+
#include "Global.h"
#include "Main.h"
#include "MiniSat.h"
#include "PbSolver.h"

#include "board.h"

bool     opt_satlive   = true;
bool     opt_ansi      = true;
char*    opt_cnf       = NULL;
int      opt_verbosity = 1;
bool     opt_try       = false;     // (hidden option -- if set, then "try" to parse, but don't output "s UNKNOWN" if you fail, instead exit with error code 5)

SolverT  opt_solver        = st_MiniSat;
ConvertT opt_convert       = ct_Mixed;
ConvertT opt_convert_goal  = ct_Undef;
bool     opt_convert_weak  = true;
double   opt_bdd_thres     = 3;
double   opt_sort_thres    = 20;
double   opt_goal_bias     = 3;
Int      opt_goal          = Int_MAX;
Command  opt_command       = cmd_Minimize;
bool     opt_branch_pbvars = false;
int      opt_polarity_sug  = 1;

char*    opt_input  = NULL;
char*    opt_result = NULL;



void reportf(const char* format, ...)
{
    static bool col0 = true;
    static bool bold = false;
    va_list args;
    va_start(args, format);
    char* text = vnsprintf(format, args);
    va_end(args);

    for(char* p = text; *p != 0; p++){
        if (col0 && opt_satlive)
            putchar('c'), putchar(' ');

        if (*p == '\b'){
            bold = !bold;
            if (opt_ansi)
                putchar(27), putchar('['), putchar(bold?'1':'0'), putchar('m');
            col0 = false;
        }else{
            putchar(*p);
            col0 = (*p == '\n' || *p == '\r');
        }
    }
    fflush(stdout);
}

void testBoard ()
{
  std::cout << "testBoard start..." << std::endl;
  Board b (3, 3);
  b. printBoard ();
  std::cout << "testBoard done." << std::endl;
}

Board* makeBoard ()
{
  int nRows=0, nCols=0;
  std::cout << "Board #Rows? "; std::cin >> nRows;
  std::cout << "Board #Cols? "; std::cin >> nCols;
  Board *p_board = new Board (nCols, nRows);
  while (1) {
    int c0, r0, c1, r1;
    std::cout << "Net col0 (-1 to end)? "; std::cin >> c0;
    if (c0 == -1) break;
    std::cout << "Net row0            ? "; std::cin >> r0;
    std::cout << "Net col1            ? "; std::cin >> c1;
    std::cout << "Net row1            ? "; std::cin >> r1;
    int netId = p_board-> addNet (c0, r0, c1, r1);
    if (netId == -1) {
      std::cout << "Invalid net terminals" << std::endl;
    }
  }
  std::cout << "Board created. #Nets " << p_board-> numNets () << "\n";
  p_board-> printBoard ();
  return p_board;
}

void formulate (const Board& b, PbSolver& solver)
{
  // vars: nNets * nVerts + nNets * nEdges
  // Nets: A, B, C, ...
  // Verts: x0, x1, .. xn
  // Edges: y01, y0n, y12, y1n+1
  //std::vector <std::string> verts, edges;
  std::vector <int> vertVars, edgeVars;

  std::map <int, std::string> varNameMap;

  //solver. allocConstrs (b. getNumVerts * b. getNumEdges * b. numNets, );

  // build vars for net-on-verts
  for (int net=0; net < b. numNets (); ++net) {
    std::string netName (1, 'A' + net);

    for (int v=0; v < b. numVerts (); ++v) {
      bool netOnVert = b. getNetOnVert (v) == net;

      std::string vert_varName = netName + b. getVertName (v);
      int vert_varInd = solver. getVar (vert_varName. c_str ());
      vertVars. push_back (vert_varInd);
      //varNameMap [vert_varInd] = vert_varName;
      varNameMap. insert (std::make_pair (vert_varInd, vert_varName));
      std::cout << "varNameMap [" << vert_varInd << "] " << vert_varName << std::endl;

      int nEdges = b. getEdges (v). size ();

      vec<Int> cs;  // co-effs
      vec<Lit> ps;  // variable
      // -nEdges *varInd + 1*edgeVarInd...
      if (netOnVert) {
        // TODO 1*vert=1; \sigma 1*verEdges=1
        cs. push (Int (-(nEdges - 1)));
      } else {
        cs. push (Int (-nEdges));
      }
      ps. push (Lit (vert_varInd));

      for (auto ve : b. getEdges (v)) {
        std::string edge_varName = netName + b. getEdgeName (ve);
        int edge_varInd = solver. getVar (edge_varName. c_str ());

        if (varNameMap. find (edge_varInd) == varNameMap. end ()) {
          edgeVars. push_back (edge_varInd);
          //varNameMap [edge_varInd] = edge_varName;
          varNameMap. insert (std::make_pair (edge_varInd, edge_varName));
          std::cout << "varNameMap [" << edge_varInd << "] " << edge_varName << std::endl;
        } else {
          std::cout << "Var " << edge_varInd << " " << edge_varName << " not-added. found " 
            << varNameMap [edge_varInd] << std::endl;

        }

        cs. push (Int (1));
        ps. push (Lit (edge_varInd));
        //std::cout << "ps push " << edge_varInd << " " << var (Lit(edge_varInd)) << std::endl;
      }

      Int rhs (0);
      int ineq = 0; // ==
      solver. addConstr (ps, cs, rhs, ineq);

      {
        std::cout << "constr:" << std::endl;
        for (int i=0; i<cs.size (); ++i) {
          int varInd = var (ps[i]);
          const std::string& varName = varNameMap [varInd];
          std::cout << "  " <<  toint (cs [i]) << "*" << varName << "(" << varInd << ") ";
        }
        std::cout << std::endl;
      }
    }
  }

  vec<Int> goal_cs;  // co-effs
  vec<Lit> goal_ps;  // variable
  for (auto edge_varInd : edgeVars) {
    goal_cs. push (Int (1));
    goal_ps. push (Lit (edge_varInd));
  }
  solver. addGoal (goal_ps, goal_cs);
}

void solve (const Board& b, PbSolver& solver)
{
  solver. solve (PbSolver::sc_Minimize);
  if (solver. best_goalvalue == Int_MAX) {
    std::cout << "Unsatisfiable" << std::endl;
  } else {
    std::cout << "Optimum found" << std::endl;
    for (int i=0; i < solver. best_model. size (); ++i) {
      std::cout << (solver. best_model [i]?"":"-") << solver. index2name[i] << std::endl;
    }
  }
}

main ()
{
  //testBoard ();

  Board *p_board = makeBoard ();
  if (!p_board) exit (1);

  PbSolver *p_solver = new PbSolver ();
  formulate (*p_board, *p_solver);
  solve (*p_board, *p_solver);

  delete p_solver;
  delete p_board;
}
