// ******************************** Includes ******************************** //

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

// **************************** Source contents ***************************** //

using namespace std;

unsigned long DJCPB(
    const vector<vector<unsigned long>>& problem,
    vector<vector<vector<unsigned long>>>& dptable)
{
  int n_factors = problem.size();
  assert(n_factors > 0);
  int n_features = problem[0].size();
  assert(n_features >= 2);
  for (int j = 0; j < n_factors; j++)
  {
    for (int i = j; i >= 0; i--)
    {
      if (i == j)
      {
        dptable[j][i] = vector<unsigned long>(n_features, 0);
      }
      else
      {
        for (int k = i + 1; k <= j; k++)
        {
          unsigned long cost = dptable[j][k][0] + dptable[k - 1][i][0] +
                               problem[j][0] * problem[k][1] * problem[i][1];
          if (k == i + 1 || cost < dptable[j][i][0])
          {
            dptable[j][i][0] = cost;
            dptable[j][i][1] = k;
          }
        }
      }
    }
  }

  return dptable[n_factors - 1][0][0];
}

unsigned long GDJCPB(
    const vector<vector<unsigned long>>& problem,
    vector<vector<vector<unsigned long>>>& dptable)
{
  int n_factors = problem.size();
  assert(n_factors > 0);
  // auxiliray names
  const unsigned int m = 0, n = 1, n_E = 2;
  const unsigned int opt_cost = 0, split_pos = 1, op_type = 2;
  const unsigned int MxM = 0, GxM = 1, MxG = 2;
  // dynamic programming
  for (int j = 0; j < n_factors; j++)
  {
    for (int i = j; i >= 0; i--)
    {
      if (i == j)
      {
        dptable[j][i][opt_cost] = problem[j][n_E] *
                                  min(problem[j][m], problem[j][n]);
        dptable[j][i][split_pos] = 0;
        if (problem[j][m] < problem[j][n])
        {
          dptable[j][i][op_type] = MxG;
        }
        else
        {
          dptable[j][i][op_type] = GxM;
        }
      }
      else
      {
        for (int k = i + 1; k <= j; k++)
        {
          unsigned long cost = dptable[j][k][opt_cost] +
                               dptable[k - 1][i][opt_cost] +
                               problem[j][m] * problem[k][n] * problem[i][n];
          if (k == i + 1 || cost < dptable[j][i][opt_cost])
          {
            dptable[j][i][opt_cost] = cost;
            dptable[j][i][split_pos] = k;
            dptable[j][i][op_type] = MxM;
          }
          cost = dptable[k - 1][i][opt_cost];
          unsigned long depth = 0;
          for (int kk = k; kk <= j; kk++)
          {
            depth += problem[kk][n_E];
          }
          cost += problem[i][n] * depth;
          if (cost < dptable[j][i][opt_cost])
          {
            dptable[j][i][opt_cost] = cost;
            dptable[j][i][op_type] = GxM;
          }
          cost = dptable[j][k][opt_cost];
          depth = 0;
          for (int kk = i; kk < k; kk++)
          {
            depth += problem[kk][n_E];
          }
          cost += problem[j][m] * depth;
          if (cost < dptable[j][i][opt_cost])
          {
            dptable[j][i][opt_cost] = cost;
            dptable[j][i][op_type] = MxG;
          }
        }
      }
    }
  }

  return dptable[n_factors - 1][0][opt_cost];
}

int main(int c, char* v[])
{
  if (c != 2)
  {
    std::cerr << "Invalid amout of arguments. Expected 2.\n";
    return EXIT_FAILURE;
  }

  ifstream in(v[1]);
  int n_factors;
  in >> n_factors;  // number of factors in chain
  const int n_features = 3, n_results = 3;
  // problem description
  // per factor: m, n, n_E
  // order in matrix chain right to left
  vector<vector<unsigned long>> problem(
      n_factors, vector<unsigned long>(n_features));
  // auxiliray names
  const unsigned int m = 0, n = 1, n_E = 2;
  // read problem description from file
  for (int i = 0; i < n_factors; i++)
  {
    in >> problem[i][m] >> problem[i][n] >> problem[i][n_E];
  }
  // print problem description
  /*
  cout << "Factors [ m n n_E ]:" << endl;
  for (int i=0;i<n_factors;i++) {
    cout << "G_{" << i << "}=[ ";
    for (int j=0;j<n_features;j++) cout << problem[i][j] << " ";
    cout << "]" << endl;
  }
  */
  // dynamic programming table
  // n_factors x n_factors lower triangular matrix (upper triangle unused)
  // per subchain: optimal cost, split position, type of operation
  // type of operation: 0: MxM, 1: GxM (tangent), 2: MxG (adjoint)
  vector<vector<vector<unsigned long>>> dptable(
      n_factors, vector<vector<unsigned long>>(
                     n_factors, vector<unsigned long>(n_results)));
  GDJCPB(problem, dptable);
  // print final dptable
  const unsigned int opt_cost = 0, split_pos = 1, op_type = 2;
  const unsigned int MxM = 0, GxM = 1, MxG = 2;
  cout << endl << "Dynamic Programming Table:" << endl;
  for (int j = 0; j < n_factors; j++)
  {
    for (int i = j; i >= 0; i--)
    {
      cout << "C_{" << j + 1 << "," << i + 1 << "}=" << dptable[j][i][opt_cost]
           << "; "
           << "Split=" << dptable[j][i][split_pos] << "; "
           << "Operation=";

      switch (dptable[j][i][op_type])
      {
        case MxM:
        {
          cout << "Preaccumulation";
        }
        break;
        case MxG:
        {
          cout << "Adjoint";
        }
        break;
        case GxM:
        {
          cout << "Tangent";
        }
        break;
        default:
        {
          assert(false);
        }
      }
      cout << endl;
    }
  }
  cout << endl
       << "Optimal Cost=" << dptable[n_factors - 1][0][opt_cost] << endl;
  // total number of edges
  int total_n_E = 0;
  for (int i = 0; i < n_factors; i++)
  {
    total_n_E += problem[i][n_E];
  }
  // tangent mode (global GxM)
  cout << endl
       << "Cost of homogeneous tangent mode=" << problem[0][n] * total_n_E
       << endl;
  // adjoint mode (global MxG)
  cout << "Cost of homogeneous adjoint mode="
       << problem[n_factors - 1][m] * total_n_E << endl;
  // preaccumulation mode (for global MxM)
  int total_preacc_cost = 0;
  for (int i = 0; i < n_factors; i++)
  {
    total_preacc_cost += problem[i][n_E] * min(problem[i][m], problem[i][n]);
  }
  int DJCPB_cost = DJCPB(problem, dptable);
  cout << "Cost of optimal homogeneous global preaccumulation="
       << total_preacc_cost << "+" << DJCPB_cost << "="
       << total_preacc_cost + DJCPB_cost << endl;
  return 0;
}
