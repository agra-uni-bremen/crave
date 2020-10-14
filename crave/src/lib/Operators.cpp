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


#include "../crave/frontend/Operators.hpp"
#include <ostream>

namespace crave {

template <typename OUT>
OUT& operator<<(OUT& out, placeholder_tag const& tag) {
  out << "placeholder<" << tag.id << ">";
  return out;
}
template std::ostream& operator<<(std::ostream& out, placeholder_tag const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_inside const&) {
  return out << "inside";
}
template std::ostream& operator<<(std::ostream& out, operator_inside const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_dist const&) {
  return out << "dist";
}
template std::ostream& operator<<(std::ostream& out, operator_dist const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_if_then const&) {
  return out << "if_then";
}
template std::ostream& operator<<(std::ostream& out, operator_if_then const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_if_then_else const& tag) {
  return out << static_cast<operator_if_then>(tag) << "_else";
}
template std::ostream& operator<<(std::ostream& out, operator_if_then_else const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_foreach const&) {
  return out << "foreach";
}
template std::ostream& operator<<(std::ostream& out, operator_foreach const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_unique const&) {
  return out << "unique";
}
template std::ostream& operator<<(std::ostream& out, operator_unique const& tag);

template <typename OUT>
OUT& operator<<(OUT& out, operator_bitslice const&) {
  return out << "bitslice";
}
template std::ostream& operator<<(std::ostream& out, operator_bitslice const& tag);
}
