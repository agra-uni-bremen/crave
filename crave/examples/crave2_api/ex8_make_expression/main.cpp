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
using crave::reference;

template <int N>
struct item : public rand_obj {
  crave::expression sum() {
    crave::expression result = crave::value_to_expression(0);
    for (int i = 0; i < N; i++) result = crave::make_expression(result + arr[i]());
    return result;
  }

  item(rand_obj* parent = 0) : rand_obj(parent) {
    for (int i = 0; i < N; i++) constraint(10 > arr[i]());
    constraint(sum() == 20);
  }

  randv<unsigned> arr[N];
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");

  item<5> it;
  CHECK(it.next());
  for (int i = 0; i < 5; i++) std::cout << it.arr[i] << " ";
  std::cout << std::endl;

  item<15> it1;
  CHECK(it1.next());
  for (int i = 0; i < 15; i++) std::cout << it1.arr[i] << " ";
  std::cout << std::endl;

  return 0;
}
