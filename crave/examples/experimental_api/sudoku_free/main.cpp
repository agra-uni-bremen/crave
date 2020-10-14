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
#include <crave/experimental/Experimental.hpp>

using crave::crv_sequence_item;
using crave::crv_constraint;
using crave::crv_variable;
using crave::crv_object_name;

using crave::inside;

class sudoku : public crv_sequence_item {
 public:
  crv_variable<short> field[9][9];  // variable to store solved sudoku

  crv_constraint c_possible_values{ "possible_values" };
  crv_constraint c_rows{ "rows" };
  crv_constraint c_columns{ "columns" };
  crv_constraint c_regions{ "regions" };

  sudoku(crv_object_name) {
    // constraint possible_values, only number from 1 to 9 are allowed
    short numbers[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++) c_possible_values &= { inside(field[i][j](), numbers) };

    // constraint rows, every number must appear exactly one time in one row
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        for (int k = j + 1; k < 9; k++) c_rows &= { field[i][j]() != field[i][k]() };

    // constraint columns, every number must appear exactly one time in one column
    for (int j = 0; j < 9; j++)
      for (int i = 0; i < 9; i++)
        for (int k = i + 1; k < 9; k++) c_columns &= { field[i][j]() != field[k][j]() };

    // constraint regions, every number must appear exactly one time in one region
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++) {
        c_regions &= { field[i][j]() != field[(i + 1) % 3 + i - (i % 3)][j](),
                       field[i][j]() != field[(i + 2) % 3 + i - (i % 3)][j](),
                       field[i][j]() != field[i][(j + 1) % 3 + j - (j % 3)](),
                       field[i][j]() != field[(i + 1) % 3 + i - (i % 3)][(j + 1) % 3 + j - (j % 3)](),
                       field[i][j]() != field[(i + 2) % 3 + i - (i % 3)][(j + 1) % 3 + j - (j % 3)](),
                       field[i][j]() != field[i][(j + 2) % 3 + j - (j % 3)](),
                       field[i][j]() != field[(i + 1) % 3 + i - (i % 3)][(j + 2) % 3 + j - (j % 3)](),
                       field[i][j]() != field[(i + 2) % 3 + i - (i % 3)][(j + 2) % 3 + j - (j % 3)]() };
      }
  }

  void print() {
    std::cout << "Sudoku solution: " << std::endl;
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++) {
        std::cout << field[i][j];
        if (j == 8) {
          if ((i == 2) || (i == 5)) std::cout << std::endl;
          std::cout << std::endl;
        } else if ((j == 2) || (j == 5))
          std::cout << "   ";
        else
          std::cout << "  ";
      }
    std::cout << std::endl;
  }
};

int main(int argc, char *argv[]) {
  crave::init("crave.cfg");
  sudoku s("random_sudoku");
  for (int i = 0; i < 10; i++) {
    CHECK(s.randomize());
    s.print();
  }
  return 0;
}
