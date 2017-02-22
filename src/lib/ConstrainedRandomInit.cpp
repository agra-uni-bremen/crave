// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#include <glog/logging.h>
#include <boost/filesystem.hpp>

#include <string>

#include "../crave/ConstrainedRandom.hpp"
#include "../crave/utils/Settings.hpp"

namespace crave {

std::string config_file_name = "crave.cfg";
std::string const& get_config_file_name() { return config_file_name; }
void set_config_file_name(std::string const& str) { config_file_name = str; }

void init() { init(get_config_file_name()); }

void init(std::string const& cfg_file) {
  CraveSetting cSettings(cfg_file);
  cSettings.load();

  set_global_seed(cSettings.get_specified_seed());
  set_solver_backend(cSettings.get_backend());
  set_config_file_name(cfg_file);

  // create the specified directory for logs
  namespace fs = boost::filesystem;
  fs::path fs_log_dir(settings.dirname());
  if (!fs::exists(fs_log_dir)) fs::create_directory(fs_log_dir);

  // initalize glog
  LoggerSetting settings(cfg_file);

  settings.load();
  FLAGS_log_dir = settings.dirname();
  FLAGS_max_log_size = settings.filesize();
  FLAGS_minloglevel = settings.s_level();
  FLAGS_logtostderr = false;
  FLAGS_logbufsecs = 0;

  static bool initialized = false;
  if (!initialized) {
    google::InitGoogleLogging(settings.filename().c_str());
    initialized = true;
  }

  cSettings.set_used_seed(rng.get_seed());
  cSettings.save();
  settings.save();
}

}  // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
