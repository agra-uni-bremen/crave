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

#include "crave/ConstrainedRandom.hpp"

#include <boost/format.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <iostream>
#include <vector>

using namespace crave;

enum kkind_e { SINGLE, INCR, WRAP4, INCR4, WRAP8, INCR8, WRAP16, INCR16 };
CRAVE_ENUM(kkind_e, (SINGLE)(INCR)(WRAP4)(INCR4)(WRAP8)(INCR8)(WRAP16)(INCR16));

enum ssize_e { BYTE, HALFWORD, WORD };
CRAVE_ENUM(ssize_e, (BYTE)(HALFWORD)(WORD));

class ahb_burst : public rand_obj {
 public:
  randv<kkind_e> kkind;
  randv<ssize_e> ssize;
  randv<unsigned int> incr_length;
  randv<unsigned int> num_transfers;
  randv<unsigned int> nnum_bytes;
  randv<unsigned int> address;

  ahb_burst(rand_obj* parent = 0)
      : rand_obj(parent),
        kkind(this),
        ssize(this),
        incr_length(this),
        num_transfers(this),
        nnum_bytes(this),
        address(this) {
    constraint("word_aligned", if_then(ssize() == WORD, address() % 4 == 0));
    constraint("halfword_aligned", if_then(ssize() == HALFWORD, address() % 2 == 0));

    constraint("legal_incr_ssize", incr_length() > 0);

    constraint("single_xfer", if_then(kkind() == SINGLE, num_transfers() == 1));
    constraint("inc_xfer", if_then(kkind() == INCR, num_transfers() == incr_length()));
    constraint("4B", if_then(kkind() == WRAP4 || kkind() == INCR4, num_transfers() == 4));
    constraint("8B", if_then(kkind() == WRAP8 || kkind() == INCR8, num_transfers() == 8));
    constraint("16B", if_then(kkind() == WRAP16 || kkind() == INCR16, num_transfers() == 16));

    constraint(nnum_bytes() == (num_transfers() << ssize()));
  }

  void print() {
    std::cout << boost::format("address=0x%x, ") % address;
    std::cout << boost::format("kkind=%u[%x], ") % kkind % kkind;
    std::cout << boost::format("ssize=%u[%x], ") % ssize % ssize;
    std::cout << boost::format("num_transfers=%u[%x], ") % num_transfers % num_transfers;
    std::cout << boost::format("nnum_bytes=%u[%x], ") % nnum_bytes % nnum_bytes;
    std::cout << std::endl;
  }
};

class mem_burst_16 : public rand_obj {
 public:
  std::vector<boost::shared_ptr<ahb_burst> > bursts;
  randv<unsigned int> legal_size;
  randv<unsigned int> bursts_nnum_bytes_acc[16];

  mem_burst_16(rand_obj* parent = 0) : rand_obj(parent), legal_size(this) {
    for (int i = 0; i < 16; i++) 
      bursts.push_back(boost::make_shared<ahb_burst>(this));

    constraint("legal_size", 1 <= legal_size() && legal_size() <= 16);

    constraint(bursts[0]->address() == 0);
    for (int i = 1; i < 16; i++) {
      constraint(bursts[i]->address() == bursts[i - 1]->address() + bursts[i - 1]->nnum_bytes());
    }

    for (int i = 0; i < 16; i++) {
      constraint(if_then(bursts[i]->kkind() == 1, bursts[i]->incr_length() <= 16));
    }

    constraint(bursts_nnum_bytes_acc[0]() == bursts[0]->nnum_bytes());
    for (int i = 1; i < 16; i++) {
      constraint(bursts_nnum_bytes_acc[i]() == bursts_nnum_bytes_acc[i - 1]() + bursts[i]->nnum_bytes());
    }

    for (int i = 0; i < 16; i++) {
      constraint(if_then(legal_size() - 1 == i, bursts_nnum_bytes_acc[i]() == 16));
    }
  }
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");

  mem_burst_16 ahb_seq;

  ahb_burst ab;

  for (int i = 0; i < 5; i++) {
    CHECK(ab.next());
    std::cout << boost::format("ab[%u]") % i << std::endl;
    ab.print();
  }

  std::cout << "-----------" << std::endl;

  for (int i = 0; i < 10; i++) {
    CHECK(ahb_seq.next());
    std::cout << boost::format("burst_size = %d") % ahb_seq.legal_size << std::endl;
    for (int j = 0; j < ahb_seq.legal_size; j++) {
      ahb_seq.bursts[j]->print();
    }
    std::cout << std::endl << std::endl << std::endl << "********" << std::endl;
  }

  return 0;
}
