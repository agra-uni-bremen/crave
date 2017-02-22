#include "../crave/utils/SettingsType.hpp"

#include <boost/property_tree/xml_parser.hpp>
#include <boost/version.hpp>

namespace crave {

#if (BOOST_VERSION >= 105600)
typedef boost::property_tree::xml_writer_settings<std::string> xml_writer_settings;
#else
typedef boost::property_tree::xml_writer_settings<char> xml_writer_settings;
#endif

Setting::Setting(std::string const& filename) : filename_(filename) {}

void Setting::load() {
  ptree tree = read_setting_file_();
  load_(tree);
}

void Setting::save() const {
  ptree tree = read_setting_file_();
  save_(&tree);
  write_xml(filename_, tree, std::locale(), xml_writer_settings('\t', 1));
}

Setting::ptree Setting::read_setting_file_() const {
  ptree tree;

  std::ifstream cfg_file(filename_.c_str());
  if (cfg_file.is_open()) read_xml(cfg_file, tree, boost::property_tree::xml_parser::trim_whitespace);

  cfg_file.close();
  return tree;
}

LoggerSetting::LoggerSetting(std::string const& filename)
    : Setting(filename),
      module_name_("logger"),
      file_(),
      dir_(),
      s_level_(),
      file_size_(),
      modules_(),
      FILE("filename"),
      DIR("directory"),
      S_LEVEL("level"),
      FILE_SIZE("filesize"),
      MODULES("modules") {}

void LoggerSetting::load_(const ptree& tree) {
  file_ = tree.get(module_name_ + "." + FILE, "crave");
  dir_ = tree.get(module_name_ + "." + DIR, "./logs");
  s_level_ = tree.get(module_name_ + "." + S_LEVEL, 2);
  file_size_ = tree.get(module_name_ + "." + FILE_SIZE, 100);
}

void LoggerSetting::save_(ptree* tree) const {
  tree->put(module_name_ + "." + FILE, file_);
  tree->put(module_name_ + "." + DIR, dir_);
  tree->put(module_name_ + "." + S_LEVEL, s_level_);
  tree->put(module_name_ + "." + FILE_SIZE, file_size_);
}

std::string const& LoggerSetting::filename() const { return file_; }

std::string const& LoggerSetting::dirname() const { return dir_; }

int LoggerSetting::s_level() const { return s_level_; }

int LoggerSetting::filesize() const { return file_size_; }
}
