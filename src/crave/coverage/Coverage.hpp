#pragma once

#include "../ir/UserConstraint.hpp"
#include "../utils/Evaluator.hpp"

#include <boost/foreach.hpp>

#include <glog/logging.h>

#include <ostream>
#include <string>
#include <vector>

namespace crave {

class covergroup;
class coverpoint;
class coverbin;

class coverbin {
public:  
  template<typename Expr>
  coverbin(Expr expr, unsigned min = 1) : bin_expr_(make_expression(expr)), hit_minimum_(min), hit_count_(0) { }

  expression bin_expr() { return bin_expr_; }
  unsigned hit_minimum() { return hit_minimum_; }
  void hit_minimum(unsigned min) { hit_minimum_ = min; }
  unsigned hit_count() { return hit_count_; }
  void hit() { hit_count_++; }
  bool covered() { return hit_count_ >= hit_minimum_; }

private:
  expression bin_expr_;
  unsigned hit_minimum_;
  unsigned hit_count_;
};

class coverpoint {
public:
  coverpoint(std::string name) : name_(name), bins_() { }  

  std::string name() { return name_; }
  std::vector<coverbin>& bins() { return bins_; }

  static coverpoint cross(coverpoint cp1, coverpoint cp2) {
    coverpoint res(std::string("cross_") + cp1.name() + "_" + cp2.name());
    BOOST_FOREACH ( coverbin& cb1 , cp1.bins() ) {
      BOOST_FOREACH ( coverbin& cb2 , cp2.bins() ) {
        res.bins().push_back(coverbin(cb1.bin_expr() && cb2.bin_expr()));
      }
    }
    return res;
  }
  
  bool covered() { 
    BOOST_FOREACH(coverbin& cb, bins_)
      if (!cb.covered())
        return false;
    return true;    
  }
  
private:
  std::vector<coverbin> bins_;
  std::string name_;  
};


class covergroup {
public:
  covergroup(std::string name) : name_(name), points_() { }
  
  std::string name() { return name_; }
  std::vector<coverpoint>& points() { return points_; }
  Evaluator& eval() { return eval_; }
  
  void sample() {
    BOOST_FOREACH ( coverpoint& cp , points_ ) {
      BOOST_FOREACH ( coverbin& cb , cp.bins() ) {
        if (eval_.evaluate(cb.bin_expr()) && eval_.result<bool>())
          cb.hit();
      }
    }
  }
  
  void report() {
    BOOST_FOREACH ( coverpoint& cp , points_ ) {
      std::cout << "Coverpoint " << cp.name() << std::endl;
      int c = 0;
      BOOST_FOREACH ( coverbin& cb , cp.bins() ) {
        std::cout << "  Coverbin " << c++ << ": " << cb.hit_count() << " " << cb.hit_minimum() << std::endl;
      }
    }  
  }

  bool covered() { 
    BOOST_FOREACH(coverpoint& cp, points_)
      if (!cp.covered())
        return false;
    return true;    
  }

private:
  std::vector<coverpoint> points_;
  std::string name_;
  Evaluator eval_;
};

} // end namespace crave
