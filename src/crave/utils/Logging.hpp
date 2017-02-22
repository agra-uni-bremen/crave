// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#ifdef CRAVE_HAVE_GLOG

#include <glog/logging.h>

#else

#include <iostream>
#include <stdexcept>

namespace crave {
enum severity_e {
  INFO = 0,
  WARNING = 1,
  ERROR = 2,
  FATAL = 3
};

extern int logging_severity;
}

#define LogStream(severity) if (severity < crave::logging_severity) {} else std::cerr
#define LOG(severity) \
    LogStream(severity) << std::endl; \
    if (severity == crave::FATAL) \
      throw std::runtime_error("Execution terminated by LOG(FATAL), log message is lost"); \
    LogStream(severity)

#define CHECK(condition) if (!(condition)) throw std::runtime_error("check failed: " #condition)

#endif
