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

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <set>

namespace crave {

class Setting {
 protected:
  typedef boost::property_tree::ptree ptree;

 protected:
  explicit Setting(std::string const& filename);

 public:
  void load();
  void save() const;

 private:
  virtual void load_(const ptree& tree) = 0;
  virtual void save_(ptree* tree) const = 0;

  ptree read_setting_file_() const;

 protected:
  std::string module_name_;  // stores module name for config file

 private:
  std::string filename_;  // stores file name of config file
};

class LoggerSetting : public Setting {
 public:
  explicit LoggerSetting(std::string const& filename);

 private:
  virtual void load_(const ptree& tree);
  virtual void save_(ptree* tree) const;

 public:
  std::string const& filename() const;
  std::string const& dirname() const;
  int s_level() const;
  int filesize() const;

 private:
  std::string module_name_;

 private:
  std::string file_;               // log filename
  std::string dir_;                // logs directory
  int s_level_;                    // severity level
  int file_size_;                  // maximum size of logfile
  std::set<std::string> modules_;  // modules where logging is enabled

 private:
  std::string const FILE;
  std::string const DIR;
  std::string const S_LEVEL;
  std::string const FILE_SIZE;
  std::string const MODULES;
};
}  // namespace crave
