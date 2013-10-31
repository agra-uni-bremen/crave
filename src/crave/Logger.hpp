#pragma once

#include <glog/logging.h>

#include <ostream>
#include <string>

class Logger {

  typedef google::LogSeverity LogSeverity;

  enum {INFO = google::INFO,
        WARNING = google::WARNING,
        ERROR = google::ERROR,
        FATAL = google::FATAL} Severity;

public:
  Logger() : lvl_(INFO) {
    set_log_size(100);
    set_log_file("./crave.log");
  }

  Logger& operator <<(std::string const& msg) {
    LOG(INFO) << msg;
    return *this;
  }

  // set log file size in MB
  void set_log_size(int const size) {
    fLI::FLAGS_max_log_size = size;
  }
  // set log file
  void set_log_file(std::string const path) {
    fLS::FLAGS_log_dir = path;
  }
  // set severity level
  void set_severity_lvl(LogSeverity const lvl) {
    lvl_ = lvl;
  }

private:
  LogSeverity lvl_;
};
