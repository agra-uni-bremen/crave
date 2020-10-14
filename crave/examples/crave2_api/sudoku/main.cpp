/****************************************************************************************
//	MIT License
//***************************************************************************************
//	Copyright (c) 2012-2020 University of Bremen, Germany. 
//  	Copyright (c) 2015-2020 DFKI GmbH Bremen, Germany.
//  	Copyright (c) 2020 Johannes Kepler University Linz, Austria.
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files (the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions:
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.
****************************************************************************************/

#include <crave/ConstrainedRandom.hpp>

using crave::rand_obj;
using crave::randv;
using crave::rand_vec;
using crave::reference;

class sudoku : public rand_obj {
 public:
  randv<int> result_sudoku[9][9];  // variable to store solved sudoku
  int given_sudoku[9][9];          // variable to hold given sudoku

  sudoku(rand_obj* parent = 0) : rand_obj(parent) {
    // constraint keep_given_sudoku, needed to copy given values to solved sudoku
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        constraint(
            crave::if_then(reference(given_sudoku[i][j]) != 0, result_sudoku[i][j]() == reference(given_sudoku[i][j])));

    // constraint possible_values, only number from 1 to 9 are allowed
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++) constraint((result_sudoku[i][j]() >= 1) && (result_sudoku[i][j]() <= 9));

    // constraint rows, every number must appear exactly one time in one row
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        for (unsigned k = j + 1; k < 9; k++) constraint(result_sudoku[i][j]() != result_sudoku[i][k]());

    // constraint columns, every number must appear exactly one time in one column
    for (int j = 0; j < 9; j++)
      for (int i = 0; i < 9; i++)
        for (int k = i + 1; k < 9; k++) constraint(result_sudoku[i][j]() != result_sudoku[k][j]());

    // constraint regions, every number must appear exactly one time in one region
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        constraint(result_sudoku[i][j]() != result_sudoku[(i + 1) % 3 + i - (i % 3)][j]())(
            result_sudoku[i][j]() != result_sudoku[(i + 2) % 3 + i - (i % 3)][j]())

            (result_sudoku[i][j]() != result_sudoku[i][(j + 1) % 3 + j - (j % 3)]())(
                result_sudoku[i][j]() != result_sudoku[(i + 1) % 3 + i - (i % 3)][(j + 1) % 3 + j - (j % 3)]())(
                result_sudoku[i][j]() != result_sudoku[(i + 2) % 3 + i - (i % 3)][(j + 1) % 3 + j - (j % 3)]())

            (result_sudoku[i][j]() != result_sudoku[i][(j + 2) % 3 + j - (j % 3)]())(
                result_sudoku[i][j]() != result_sudoku[(i + 1) % 3 + i - (i % 3)][(j + 2) % 3 + j - (j % 3)]())(
                result_sudoku[i][j]() != result_sudoku[(i + 2) % 3 + i - (i % 3)][(j + 2) % 3 + j - (j % 3)]());
  }

  void solve(int given[9][9]) {
    int count = 0;

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++) {
        given_sudoku[i][j] = given[i][j];
        if (given_sudoku[i][j] != 0) count++;
      }

    std::cout << "Number of given fields: " << count << std::endl;
    std::cout << "Solution:" << std::endl;

    if (next()) {  // run the solver
      for (int i = 0; i < 9; i++)
        for (int j = 0; j < 9; j++) {
          std::cout << result_sudoku[i][j];
          if (j == 8) {
            if ((i == 2) || (i == 5)) std::cout << std::endl;
            std::cout << std::endl;
          } else if ((j == 2) || (j == 5))
            std::cout << "   ";
          else
            std::cout << "  ";
        }
    } else
      std::cout << "Fehler!";
  }
};

int given_sudoku[9][9] = { { 0, 0, 0, 0, 0, 0, 0, 1, 2 },
                           { 0, 5, 0, 4, 0, 0, 0, 0, 0 },
                           { 0, 0, 0, 0, 0, 0, 0, 3, 0 },

                           { 7, 0, 0, 6, 0, 0, 4, 0, 0 },
                           { 0, 0, 1, 0, 0, 0, 0, 0, 0 },
                           { 0, 0, 0, 0, 8, 0, 0, 0, 0 },

                           { 9, 2, 0, 0, 0, 0, 8, 0, 0 },
                           { 0, 0, 0, 0, 5, 1, 7, 0, 0 },
                           { 0, 0, 0, 0, 0, 3, 0, 0, 0 } };

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");
  sudoku s;
  s.solve(given_sudoku);
  return 0;
}
