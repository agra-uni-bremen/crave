// Copyright 2014 The CRAVE developers. All rights reserved.//

#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
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
