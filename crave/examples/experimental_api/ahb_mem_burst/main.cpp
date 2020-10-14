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
#include <boost/format.hpp>
#include <iostream>

using namespace crave;

CRAVE_BETTER_ENUM(kkind_e, SINGLE, INCR, WRAP4, INCR4, WRAP8, INCR8, WRAP16, INCR16);
CRAVE_BETTER_ENUM(ssize_e, BYTE, HALFWORD, WORD);

class ahb_burst : public crv_sequence_item {
 public:
  crv_variable<kkind_e> kkind;
  crv_variable<ssize_e> ssize;
  crv_variable<unsigned int> incr_length;
  crv_variable<unsigned int> num_transfers;
  crv_variable<unsigned int> nnum_bytes;
  crv_variable<unsigned int> address;

  crv_constraint aligned_address{ if_then(ssize() == ssize_e::WORD, address() % 4 == 0),
                                  if_then(ssize() == ssize_e::HALFWORD, address() % 2 == 0) };

  crv_constraint legal_incr_ssize{ incr_length() > 0 };

  crv_constraint compute_num_transfers{
    if_then(kkind() == kkind_e::SINGLE, num_transfers() == 1),
    if_then(kkind() == kkind_e::INCR, num_transfers() == incr_length()),
    if_then(kkind() == kkind_e::WRAP4 || kkind() == kkind_e::INCR4, num_transfers() == 4),
    if_then(kkind() == kkind_e::WRAP8 || kkind() == kkind_e::INCR8, num_transfers() == 8),
    if_then(kkind() == kkind_e::WRAP16 || kkind() == kkind_e::INCR16, num_transfers() == 16)
  };

  crv_constraint compute_num_bytes{ nnum_bytes() == (num_transfers() << ssize()) };

  ahb_burst(crave::crv_object_name name = "ahb_burst") {}

  void print() {
    std::cout << boost::format("address=0x%x, ") % address;
    std::cout << boost::format("kkind=%s[0x%x], ") % kkind._to_string() % kkind;
    std::cout << boost::format("ssize=%s[0x%x], ") % ssize._to_string() % ssize;
    std::cout << boost::format("num_transfers=%u[0x%x], ") % num_transfers % num_transfers;
    std::cout << boost::format("nnum_bytes=%u[0x%x], ") % nnum_bytes % nnum_bytes;
    std::cout << std::endl;
  }
};

class mem_burst_16 : public crv_sequence_item {
 public:
  ahb_burst bursts[16];
  crv_variable<unsigned int> legal_size;
  crv_variable<unsigned int> bursts_nnum_bytes_acc[16];

  crv_constraint legal_size_con{ legal_size() <= 16, legal_size() > 0 };

  crv_constraint addresses;
  crv_constraint max_16_bytes;

  mem_burst_16(crave::crv_object_name) {
    addresses &= { bursts[0].address() == 0 };
    for (int i = 1; i < 16; i++)
      addresses &= { bursts[i].address() == bursts[i - 1].address() + bursts[i - 1].nnum_bytes() };

    for (int i = 0; i < 16; i++)
      max_16_bytes &= { if_then(bursts[i].kkind() == kkind_e::INCR, bursts[i].incr_length() <= 16) };

    max_16_bytes &= { bursts_nnum_bytes_acc[0]() == bursts[0].nnum_bytes(),
                      if_then(legal_size() == 1, bursts_nnum_bytes_acc[0]() == 16) };
    for (int i = 1; i < 16; i++) {
      max_16_bytes &= { bursts_nnum_bytes_acc[i]() == bursts_nnum_bytes_acc[i - 1]() + bursts[i].nnum_bytes(),
                        if_then(legal_size() - 1 == i, bursts_nnum_bytes_acc[i]() == 16) };
    }
  }
};

int main(int argc, char* argv[]) {
  crave::init("crave.cfg");

  mem_burst_16 ahb_seq("item");

  ahb_burst ab("burst");

  for (int i = 0; i < 5; i++) {
    CHECK(ab.randomize());
    std::cout << boost::format("ab[%u]") % i << std::endl;
    ab.print();
  }

  std::cout << "-----------" << std::endl;

  for (int i = 0; i < 10; i++) {
    CHECK(ahb_seq.randomize());
    std::cout << boost::format("burst_size = %d") % ahb_seq.legal_size << std::endl;
    for (int j = 0; j < ahb_seq.legal_size; j++) {
      ahb_seq.bursts[j].print();
    }
    std::cout << std::endl << std::endl << std::endl << "********" << std::endl;
  }

  return 0;
}
