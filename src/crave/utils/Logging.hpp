// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#ifdef CRAVE_HAVE_GLOG

#include <glog/logging.h>

#else

#include <iostream>

#define LOG(INFO) std::cerr << std::endl; std::cerr

#include <stdexcept>

#define CHECK(condition) if (!(condition)) throw std::runtime_error("check failed: " #condition)

#endif
