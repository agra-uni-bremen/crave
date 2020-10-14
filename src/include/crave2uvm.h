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

/*! \file crave2uvm.h
    \brief Include this file to get all Crave2UVM functionality.

    This file contains all includes and macros you need to get started with Crave2UVM.
    It includes the two adapter classes and redefines UVM_DO_ON_PRI_WITH in order to
 *  add random verification with constraints.
 */

#pragma once
#include "uvm_randomized_sequence_item.h"
#include "uvm_randomized_sequence.h"


#define UNPACK(...) __VA_ARGS__


#undef UVM_DO
#define UVM_DO(SEQ_OR_ITEM) \
  UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, this->m_sequencer, -1,)

#undef UVM_DO_WITH
#define UVM_DO_WITH(SEQ_OR_ITEM, CONSTRAINTS) \
	UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, this->m_sequencer, -1, UNPACK CONSTRAINTS)

#undef UVM_DO_PRI
#define UVM_DO_PRI(SEQ_OR_ITEM, PRIORITY) \
  UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, this->m_sequencer, PRIORITY,)

#undef UVM_DO_ON
#define UVM_DO_ON(SEQ_OR_ITEM, SEQR) \
  UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, SEQR, -1,)

#undef UVM_DO_ON_PRI
#define UVM_DO_ON_PRI(SEQ_OR_ITEM, SEQR, PRIORITY) \
  UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, SEQR, PRIORITY,)

/*! \def UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, SEQR, PRIORITY, CONSTRAINTS)
    \brief Redefine UVM_DO_ON_PRI_WITH to use in \a CONSTRAINTS with CRAVE.
 */
#undef UVM_DO_ON_PRI_WITH
#define UVM_DO_ON_PRI_WITH(SEQ_OR_ITEM, SEQR, PRIORITY, CONSTRAINTS) \
  { \
  ::uvm::uvm_sequence_base* seq__; \
  UVM_CREATE_ON(SEQ_OR_ITEM, SEQR) \
  seq__ = dynamic_cast<uvm::uvm_sequence_base*>(SEQ_OR_ITEM); \
  if (seq__ == NULL) this->start_item(SEQ_OR_ITEM, PRIORITY); \
  if (seq__ == NULL && !SEQ_OR_ITEM->randomize_with(CONSTRAINTS)) \
    UVM_WARNING("RNDFLD", "Randomization failed in uvm_do_with action"); \
  if (seq__ == NULL) this->finish_item(SEQ_OR_ITEM, PRIORITY); \
  else seq__->start(SEQR, this, PRIORITY, 0); \
  } \

