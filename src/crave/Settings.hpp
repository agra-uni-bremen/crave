#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <fstream>
#include <set>
#include <string>

namespace crave {

class Setting {

public:
  Setting() { }

  virtual void load(std::string const& filename) = 0;
  virtual void save(std::string const& filename) const = 0;

};

class LoggerSetting : private Setting {

public:
  LoggerSetting()
  : Setting(), file_(), dir_(), s_level_(), file_size_(), modules_() { }
  LoggerSetting(std::string const& filename)
  : Setting(), file_(), dir_(), s_level_(), file_size_(), modules_() {
    load(filename);
  }

  virtual void load(std::string const& filename) {
    using boost::property_tree::ptree;
    ptree tree;

    std::ifstream cfg_file(filename.c_str());
    if (cfg_file.is_open())
      read_xml(cfg_file, tree, boost::property_tree::xml_parser::trim_whitespace);

    file_ = tree.get("logger.filename", "crave.log");
    dir_ = tree.get("logger.directory", "./logs");
    s_level_ = tree.get("logger.level", 0);
    file_size_ = tree.get("logger.filesize", 100);
  }
  virtual void save(std::string const& filename) const {
    using boost::property_tree::ptree;
    ptree tree;

    tree.put("logger.filename", file_);
    tree.put("logger.directory", dir_);
    tree.put("logger.level", s_level_);
    tree.put("logger.filesize", file_size_);

    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
    write_xml(filename, tree, std::locale(), settings);
  }

  std::string const& filename() const {
    return file_;
  }
  std::string const& dirname() const {
    return dir_;
  }
  int s_level() const {
    return s_level_;
  }
  int filesize() const {
    return file_size_;
  }

// private:
  std::string file_;              // log filename
  std::string dir_;               // logs directory
  int s_level_;                   // severity level
  int file_size_;                 // maximum size of logfile
  std::set<std::string> modules_; // modules where logging is enabled
};

} // end namespace crave