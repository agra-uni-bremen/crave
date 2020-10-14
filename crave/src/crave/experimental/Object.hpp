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

#include <iostream>
#include <list>
#include <string>
#include <unordered_map>

namespace crave {

struct Generator;

struct crv_object_name;
class crv_object;

/**
 * \ingroup newAPI
 * \brief Class for names of CRAVE objects.
 * 
 * This class is a type safe class to wrap the name of an object in CRAVE.
 */
struct crv_object_name {
  /**
   * \brief Constructor using a char array.
   * \param ca character array to create the name from.
   */
  crv_object_name(const char*);
  /**
   * \brief Constructor using a string.
   * \param s string to create the name from.
   */
  crv_object_name(const std::string& s) : crv_object_name(s.c_str()) {}
  /**
   * \brief Copy constructor.
   * \param other Other object to copy from.
   */
  crv_object_name(const crv_object_name&) = default;
  /**
   * \brief Destructor.
   */
  ~crv_object_name();

  /**
   * \brief Operator() to get the name as a string.
   * \return wrapped name as a string
   */
  std::string operator()() { return name_; }

  std::string name_;   /** < name of object*/
  crv_object* object_; /** < internal crave object*/
};

/**
 * \ingroup newAPI
 * \brief Base class for all CRAVE objects within the new API.
 */
class crv_object {
  template <typename T, typename Enable>
  friend class crv_variable;

 public:
  /**
  * \brief Empty Constructor.
  */
  crv_object();
  /**
   * \brief Destructor.
   */
  ~crv_object();

  /**
   * \brief Return the type of the object as a string.
   * 
   * The type is most likely the name of the class the object is made from.
   * 
   * \return Type of the object as a string.
   */
  virtual std::string obj_kind() const { return "crv_object"; }
  
  /**
   * \brief Randomizes the variables in this object.
   * 
   * The variables must be of type crv_variable in order to be randomized.
   * While randomizing, all constraints that are part of the object will be used to get a solution.
   * These constraints may produce contradicts and cannot be solved.
   * In that case, this method returns false.
   * 
   * \return true if a solution is found, false otherwise.
   */
  virtual bool randomize() { return false; }

  /**
   * \brief Get the name of the object.
   * \return Name of the object as a string.
   */
  std::string name() const { return name_; }

  /**
   * \brief Get the fullname of the object.
   * \return Fullname of the object as a string.
   */
  std::string fullname() const { return fullname_; }

  /**
   * outputs the object hierarchy (i.e. children and their children, etc.) to std::cout
   */
  void print_object_hierarchy(int level = 0) const;

  /**
   * get the root object of the hierarchy
   */
  static crv_object* root();

  /**
   * find the object with a given name
   */
  static crv_object* find(std::string);

  /**
   * get the numer of objects in the hierarchy
   */
  static unsigned count();

 protected:
  /**
   * copy constructor
   */
  crv_object(const crv_object& other)
      : name_(other.name_), orig_name_(other.orig_name_), parent_(), children_(), fullname_(other.fullname_) {}

  void remove_child(crv_object*);

  virtual void request_rebuild() {
    if (parent_) parent_->request_rebuild();
  }

  virtual void recursive_build(Generator& gen) const;

  std::string name_;                                         /** < name of the object*/
  std::string orig_name_;                                    /** < original name of the object*/
  crv_object* parent_;                                       /** < parent node*/
  std::list<crv_object*> children_;                          /** < children nodes*/
  std::string fullname_;                                     /** < fullname*/
  std::unordered_map<std::string, unsigned> local_name_map_; /** < names of children in a map*/

 private:
  /**
   * internal constructor, used to construct the root object
   */
  crv_object(int) : name_("root"), orig_name_(name_), parent_(), children_(), fullname_(name_) {}
};

};  // namespace crave
