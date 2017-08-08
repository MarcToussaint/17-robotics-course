/*  ------------------------------------------------------------------
    Copyright 2016 Marc Toussaint
    email: marc.toussaint@informatik.uni-stuttgart.de
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or (at
    your option) any later version. This program is distributed without
    any warranty. See the GNU General Public License for more details.
    You should have received a COPYING file of the full GNU General Public
    License along with this program. If not, see
    <http://www.gnu.org/licenses/>
    --------------------------------------------------------------  */


/* Note (mt): The Hungarian does a /perfect/ weighted bi-partite graph matching.
 * Perfect means that the mathching has full caridinality, i.e., all vertices have
 * a match (in the other partite). Non-perfect minimum weight matching can be reduced
 * to perfect matching by just expanding the graph (having O+P vertices); see also
 * (section 1.5.1 of Guido Schäfer's Master's thesis
 * http://homepages.cwi.nl/~schaefer/ftp/pdf/masters-thesis.pdf
 *
 * The Hungarian is not really efficient. More efficient implementations of graph matching are found here:
 * http://pub.ist.ac.at/~vnk/software.html#BLOSSOM5
 * and here
 * https://www.cs.purdue.edu/homes/apothen/software.html
 *
 * General info:
 * https://en.wikipedia.org/wiki/Matching_(graph_theory)
 * https://en.wikipedia.org/wiki/Blossom_algorithm
 *
 * If it is not an efficiency bottleneck, we stick with Hungarian first.
 */

#include "hungarian.h"

Hungarian::Hungarian(const arr& cost_matrix)
{
  costs = cost_matrix;
  dim = costs.dim(0);
  starred = zeros(dim, dim);
  primed = starred;
  covered_rows = zeros(dim);
  covered_cols = covered_rows;
  minimize();
}

Hungarian::~Hungarian(){}

void Hungarian::minimize()
{
  covered_rows = covered_cols = zeros(dim);
  starred = primed = zeros(dim, dim);
  for (uint i = 0; i < dim; i++ )
  {
    double minRow = costs[i]().minIndex();
    costs[i]() -= costs(i, minRow);
  }
  costs = ~costs;

  for (uint i = 0; i < dim; i++ )
  {
    double minRow = costs[i]().minIndex();
    costs[i]() -= costs(i, minRow);
  }
  costs = ~costs;
  starZeros();
}

void Hungarian::starZeros()
{
  for (uint i = 0; i < dim; i++ )
  {
    if (covered_rows(i))
      continue;

    for (uint j = 0; j < dim; j++ )
    {
      if (covered_cols(j))
        continue;

      if (costs(i,j) == 0)
      {
        starred(i,j) = 1;
        covered_rows(i) = 1;
        covered_cols(j) = 1;
        break;
      }
    }
  }

  covered_rows = zeros(dim);
  covered_cols = covered_rows;
  coverColumns();
}

void Hungarian::coverColumns()
{
  uint count = 0;
  starred = ~starred;
  for (uint i = 0; i < dim; i++ )
  {
    if (sum(starred[i]()) > 0)
    {
      covered_cols(i) = 1;
      count++;
    }
  }
  starred = ~starred;

  if (count == dim)
    return;

  prime();
}

void Hungarian::prime()
{
  // Find an uncovered zero.
  for (uint i = 0; i < dim; i++ )
  {
    if (covered_rows(i))
      continue;

    for (uint j = 0; j < dim; j++ )
    {
      if (covered_cols(j))
        continue;

      if (costs(i,j) == 0)
      {
        primed(i,j) = 1;
        // Check to see if there is a starred zero in this row.

        if (sum(starred[i]()) == 0)
        {
          path_row.clear();
          path_row.push_back(i);
          path_col.clear();
          path_col.push_back(j);
          makePath();
          return;
        }
        else
        {
          // Cover this row
          covered_rows(i) = 1;
          // Uncover columns containing star
          uint maxIndex = starred[i]().maxIndex();
          covered_cols(maxIndex) = 0;
          prime();
          return;
        }
      }
    }
  }
  modifyCost();
  return;
}

void Hungarian::makePath()
{
  uint count = 0;

  while (1)
  {
    starred = ~starred;
    // find the star in the column
    int row = starred[path_col.at(count)]().maxIndex();

    starred = ~starred;
    if (starred(row, path_col.at(count)) == 0)
      break;

    count++;
    path_row.push_back(row);
    path_col.push_back(path_col.at(count - 1));

    // find the prime in this row
    int col = primed[row]().maxIndex();
    count++;
    path_row.push_back(path_row.at(count - 1));
    path_col.push_back(col);
  }

  // Modify it.
  for (uint i = 0; i <= count; i++ )
  {
    uint row = path_row.at(i);
    uint col = path_col.at(i);

    if (starred(row,col))
    {
      starred(row,col) = 0;
    }
    else
    {
      starred(row,col) = 1;
    }
  }

  // Clear covers and primes, call cover columns.
  covered_rows = covered_cols = zeros(dim);
  primed = zeros(dim, dim);
  coverColumns();
}

void Hungarian::modifyCost()
{
  auto minCost = max(costs);
  for (uint i = 0; i < dim; i++ )
  {
    if (covered_rows(i))
      continue;

    for (uint j = 0; j < dim; j++)
    {
      if (covered_cols(j))
        continue;

      if (costs(i,j) < minCost)
        minCost = costs(i,j);
    }
  }
  // Modify the costs
  for (uint i = 0; i < dim; i++ )
  {
    for (uint j = 0; j < dim; j++)
    {
      if (covered_rows(i))
      {
        costs(i,j) += minCost;
      }
      else if (!covered_cols(j))
      {
        costs(i,j) -= minCost;
      }
    }
  }

  prime();
}
