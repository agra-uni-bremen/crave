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


#pragma once

namespace crave {

/**
 * \ingroup oldAPI
 * \ingroup newAPI
 * \brief A weighted range of values of type T
 * 
 * A weighted range is a range with an annotated weight.
 * The weight indicates the chance to be hit inside a distribution where this range is included.
 * A greater weight means the distribution will hit the range more often. 
 */
template <typename T>
struct weighted_range {
  /**
   * \brief Defines a new weighted range.
   * 
   * \param l Left value of the range (inclusive)
   * \param r Right value of the range (inclusive)
   * \param w Weight of the range
   */
  weighted_range(T l, T r, uint64_t w) : left_(l), right_(r), weight_(w), accumWeight_(0) {}

  /**
   * \brief Compares a weighted range to another range.
   * 
   * A range is less than another range, if its left value is less than the left value of the other range or its left values are equal and the right value of the range is less than the right value of the other range.
   * In all other cases, it is not.
   * 
   * \param other Other range to compare with
   * \return true if this range is less than the other range, false otherwise.
   */
  bool operator<(const weighted_range<T>& other) const {
    if (left_ < other.left_) return true;
    if (left_ > other.left_) return false;
    if (right_ < other.right_) return true;
    return false;
  }

  /**
   * \brief Checks if this range overlaps another range.
   * 
   * \param other Other range to check overlap with
   * \return true if this range overlaps the other range, false otherwise.
   */
  bool overlap(const weighted_range<T>& other) const {
    if (left_ < other.left_) return right_ >= other.left_;
    if (left_ > other.left_) return left_ <= other.right_;
    return false;
  }

  T left_;
  T right_;
  uint64_t weight_;
  uint64_t accumWeight_;
};

/**
 * \ingroup oldAPI
 * \ingroup newAPI
 * \brief A range of values of type T.
 * 
 * A range with default weight that is equal to its size.
 * For details about the weight see the base class crave::weighted_range.
 */
template <typename T>
struct range : weighted_range<T> {
  /*!
   * \brief Defines a new range from a left value to a right value.
   * 
   * \param l Left value of the range (inclusive)
   * \param r Right value of the range (inclusive)
   */
  range(T l, T r) : weighted_range<T>(l, r, r + 1 - l) {}
};

/**
 * \ingroup oldAPI
 * \ingroup newAPI
 * \brief A single value of type T with a weight.
 * 
 * A weighted value is a weighted range containing only one element.
 * For details about the weight see the base class crave::weighted_range.
 */
template <typename T>
struct weighted_value : weighted_range<T> {
  /*!
   * \brief Defines a new weighted value with its weight.
   * 
   * \param v The value.
   * \param w The weight.
   */
  weighted_value(T v, uint64_t w) : weighted_range<T>(v, v, w) {}
};
}  // namespace crave
