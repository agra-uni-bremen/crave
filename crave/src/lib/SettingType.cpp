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
