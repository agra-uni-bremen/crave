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
#include <vector>
#include <boost/timer.hpp>

using crave::rand_obj;
using crave::randv;
using crave::inside;

class sudoku : public rand_obj {
 public:
  randv<short> field[9][9];

  sudoku(rand_obj* parent = 0) : rand_obj(parent) {
    short numbers[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++) constraint(inside(field[i][j](), numbers));

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        for (int k = j + 1; k < 9; k++) constraint(field[i][j]() != field[i][k]());

    for (int j = 0; j < 9; j++)
      for (int i = 0; i < 9; i++)
        for (int k = i + 1; k < 9; k++) constraint(field[i][j]() != field[k][j]());

    for (int i = 0; i < 9; i++)
      for (int j = 0; j < 9; j++)
        constraint(field[i][j]() != field[(i + 1) % 3 + i - (i % 3)][j]())(field[i][j]() !=
                                                                           field[(i + 2) % 3 + i - (i % 3)][j]())

            (field[i][j]() != field[i][(j + 1) % 3 + j - (j % 3)]())(
                field[i][j]() != field[(i + 1) % 3 + i - (i % 3)][(j + 1) % 3 + j - (j % 3)]())(
                field[i][j]() != field[(i + 2) % 3 + i - (i % 3)][(j + 1) % 3 + j - (j % 3)]())

            (field[i][j]() != field[i][(j + 2) % 3 + j - (j % 3)]())(
                field[i][j]() != field[(i + 1) % 3 + i - (i % 3)][(j + 2) % 3 + j - (j % 3)]())(
                field[i][j]() != field[(i + 2) % 3 + i - (i % 3)][(j + 2) % 3 + j - (j % 3)]());
  }
};

template <bool SAT, unsigned N = 3>
class sudoku_container : public rand_obj {
 public:
  sudoku_container(rand_obj* parent = 0) : rand_obj(parent) {
    for (unsigned i = 0; i < N; i++) {
      list.push_back(new sudoku(this));
      if (!SAT)
        constraint(list[i]->field[0][0]() > 9);
    }
  }

 private:
  std::vector<sudoku*> list;
};

template <bool SAT, bool MT>
void run_test() {
  boost::timer timer;
  crave::init("crave.cfg");
  sudoku_container<SAT> sc;
  if (MT) sc.constraint.enable_multithreading();
  for (int i = 0; i < 10; i++) {
    std::cout << "Iteration #" << (i + 1) << std::endl;
    CHECK(sc.next() == SAT);
  }
  std::cout << "complete: " << timer.elapsed() << std::endl;
}
