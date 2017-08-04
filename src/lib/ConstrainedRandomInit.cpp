// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#if (_MSC_VER >= 1900)

#include <experimental/filesystem> // C++-standard header file name
#include <filesystem> // Microsoft-specific implementation header file name
namespace fs = std::experimental::filesystem::v1;

#else

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

#endif

#include <string>

#include "../crave/ConstrainedRandom.hpp"
#include "../crave/utils/Settings.hpp"
#include "../crave/utils/Logging.hpp"

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

  // load logger settings
  LoggerSetting settings(cfg_file);
  settings.load();

  // create the specified directory for logs
  fs::path fs_log_dir(settings.dirname());
  if (!fs::exists(fs_log_dir)) fs::create_directory(fs_log_dir);

#ifdef CRAVE_HAVE_GLOG
 // initalize glog
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
#else
  logging_severity = settings.s_level();
#endif

  // save settings
  cSettings.set_used_seed(rng.get_seed());
  cSettings.save();
  settings.save();
}

#ifndef CRAVE_HAVE_GLOG
int logging_severity;
#endif

}  // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
