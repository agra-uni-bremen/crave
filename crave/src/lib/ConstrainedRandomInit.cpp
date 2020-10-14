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
