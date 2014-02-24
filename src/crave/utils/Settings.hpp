#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <fstream>
#include <set>
#include <string>

namespace crave {

class Setting {
protected:
  typedef boost::property_tree::ptree ptree;

protected:
  Setting(std::string const& filename) : filename_(filename) { }

public:
  void load() {
    ptree tree = read_setting_file_();
    load_(tree);
  }
  void save() const {

    ptree tree = read_setting_file_();
    save_(tree);

    boost::property_tree::xml_writer_settings<char> settings('\t', 1);
    write_xml(filename_, tree, std::locale(), settings);
  }

private:
  virtual void load_(ptree&) = 0;
  virtual void save_(ptree&) const = 0;

  ptree read_setting_file_() const {
    ptree tree;

    std::ifstream cfg_file(filename_.c_str());
    if (cfg_file.is_open())
      read_xml(cfg_file, tree, boost::property_tree::xml_parser::trim_whitespace);

    cfg_file.close();
    return tree;
  }

protected:
  std::string module_name_;   // stores module name for config file
private:
  std::string filename_;      // stores file name of config file
};

class LoggerSetting : public Setting {
public:
  LoggerSetting(std::string const& filename)
  : Setting(filename), module_name_("logger"), file_(), dir_(), s_level_(), file_size_(), modules_(),
    FILE("filename"), DIR("directory"), S_LEVEL("level"), FILE_SIZE("filesize"), MODULES("modules") { }

private:
  virtual void load_(ptree& tree) {
    file_ = tree.get(module_name_ + "." + FILE, "crave");
    dir_ = tree.get(module_name_ + "." + DIR, "./logs");
    s_level_ = tree.get(module_name_ + "." + S_LEVEL, 2);
    file_size_ = tree.get(module_name_ + "." + FILE_SIZE, 100);
  }
  virtual void save_(ptree& tree) const {
    tree.put(module_name_ + "." + FILE, file_);
    tree.put(module_name_ + "." + DIR, dir_);
    tree.put(module_name_ + "." + S_LEVEL, s_level_);
    tree.put(module_name_ + "." + FILE_SIZE, file_size_);
  }

public:
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

private:
  std::string module_name_;
private:
  std::string file_;              // log filename
  std::string dir_;               // logs directory
  int s_level_;                   // severity level
  int file_size_;                 // maximum size of logfile
  std::set<std::string> modules_; // modules where logging is enabled
private:
  std::string const FILE;
  std::string const DIR;
  std::string const S_LEVEL;
  std::string const FILE_SIZE;
  std::string const MODULES;
};

class CraveSetting : public Setting {
public:
  CraveSetting(std::string const& filename)
  : Setting(filename), module_name_("crave"), backend_(), seed_(),
    BACKEND("backend"), SEED("seed") { }

private:
  virtual void load_(ptree& tree) {
    backend_ = tree.get(module_name_ + "." + BACKEND, "Boolector");
    seed_ = tree.get(module_name_ + "." + SEED, 42);
  }
  virtual void save_(ptree& tree) const {
    tree.put(module_name_ + "." + BACKEND, backend_);
    tree.put(module_name_ + "." + SEED, seed_);
  }

public:
  std::string const& get_backend() const {
    return backend_;
  }
  unsigned int get_seed() const {
    return seed_;
  }

private:
  std::string module_name_;
private:
  std::string backend_;
  unsigned int seed_;
private:
  std::string const BACKEND;
  std::string const SEED;
};

} // end namespace crave
