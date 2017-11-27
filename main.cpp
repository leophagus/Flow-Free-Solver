#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdarg>
#include <chrono>

#include "board.h"
#include "problem.h"
#include "satform.h"

extern void buildProblem (const Board& B, Problem& P);

bool     opt_satlive   = false; // 
bool     opt_ansi      = false;
char*    opt_cnf       = NULL;
int      opt_verbosity = 0;     // progress info from sat
bool     opt_try       = false; // (hidden option -- if set, then "try" to parse, but don't output "s UNKNOWN" if you fail, instead exit with error code 5)

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

int diffTimeUs (const std::chrono::time_point<std::chrono::steady_clock>& t1,
                const std::chrono::time_point<std::chrono::steady_clock>& t0)
{
  return std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
}

main (int argc, char *argv[])
{
  if (argc != 3) {
    std::cout << "Usage: router board_h board_w" << std::endl;
    return 1;
  }
  int board_h = std::stoi (std::string(argv[1]));
  int board_w = std::stoi (std::string(argv[2]));

  int dbg = 0;

  auto t0 = std::chrono::steady_clock::now();

  Board B (board_h, board_w);
  Problem P;
  buildProblem (B, P);
  auto t1 = std::chrono::steady_clock::now();

  PbSolver solver;

  SatForm S (B.getGraph (), P, solver, dbg);
  S. formulate ();
  auto t2 = std::chrono::steady_clock::now();

  bool status = S. solve ();
  auto t3 = std::chrono::steady_clock::now();

  if (status) {
    S. readSolution ();
  }

  std::cout << "Elapsed times (us) "
   << " Bld: " << diffTimeUs(t1, t0)
   << " Frm: " << diffTimeUs(t2, t1)
   << " Slv: " << diffTimeUs(t3, t2)
   << std::endl;
}
