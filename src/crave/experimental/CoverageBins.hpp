// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.

#pragma once

#include "Expression.hpp"
#include "Object.hpp"
#include "Variable.hpp"
#include "../ir/UserExpression.hpp"
#include "../utils/Evaluator.hpp"

#include <string>
#include <vector>

namespace crave {

class crv_transition_bin;

/**
 * \brief A state (or node) of a transition checking FSM used by crv_transition_bin
 * 
 * Each statement of a definition of a transition is translated into a FSM.
 * One statement is exactly one node in this FSM.
 * The logic to traverse this FSM is contained in crv_transition_bin.
 */
struct crv_transition_fsm_state : std::enable_shared_from_this<crv_transition_fsm_state> {
  friend class crv_transition_bin;
  friend class crv_coverpoint;
  
  //****************************
  // User visible continue-functions
  //****************************

  // =>
  template <typename Expr>
  std::shared_ptr<crv_transition_fsm_state> next(Expr expr) {
    return crv_transition_fsm_state::next(shared_from_this(), expr);
  }

  // [* N:M]
  template <unsigned int N, unsigned int M = N, typename Expr>
  std::shared_ptr<crv_transition_fsm_state> consecutive_next(Expr expr) {
    return crv_transition_fsm_state::consecutive_repeat<N, M>(shared_from_this(), expr);
  }

  // [-> N:M]
  template <unsigned int N, unsigned int M = N, typename Expr>
  std::shared_ptr<crv_transition_fsm_state> goto_next(Expr expr) {
    return crv_transition_fsm_state::goto_repeat<N, M>(shared_from_this(), expr);
  }

  // [= N:M]
  template <unsigned int N, unsigned int M = N, typename Expr>
  std::shared_ptr<crv_transition_fsm_state> nonconsecutive_next(Expr expr) {
    return crv_transition_fsm_state::non_consecutive_repeat<N, M>(shared_from_this(), expr);
  }

  //****************************
  // API abstract functions
  //****************************
  enum nodeType { GOTO, CONSEC, NON_CONSEC };

  template <unsigned int N = 1, unsigned int M = N, typename Expr>
  static std::shared_ptr<crv_transition_fsm_state> addState(std::shared_ptr<crv_transition_fsm_state> node, Expr expr,
                                                            nodeType pNodeType = CONSEC) {
    if (N <= 0) {
      return node;
    }
    crv_transition_fsm_state *nextNodePointer = new crv_transition_fsm_state(expr, pNodeType);
    std::shared_ptr<crv_transition_fsm_state> nextNode = std::shared_ptr<crv_transition_fsm_state>(nextNodePointer);
    nextNode->minHit = N;
    nextNode->maxHit = M;
    nextNode->prev = node;
    if (node) {
      node->succ = nextNode;
    }
    return nextNode;
  }
  
  //*1
  template <typename Expr>
  static std::shared_ptr<crv_transition_fsm_state> next(std::shared_ptr<crv_transition_fsm_state> node, Expr expr) {
    return crv_transition_fsm_state::consecutive_repeat<1>(node, expr);
  }

  //*
  template <unsigned int N, unsigned int M = N, typename Expr>
  static std::shared_ptr<crv_transition_fsm_state> consecutive_repeat(std::shared_ptr<crv_transition_fsm_state> node,
                                                               Expr expr) {
    return addState<N, M>(node, expr);
  }

  //->
  template <unsigned int N, unsigned int M = N, typename Expr>
  static std::shared_ptr<crv_transition_fsm_state> goto_repeat(std::shared_ptr<crv_transition_fsm_state> node,
                                                                      Expr expr) {
    return addState<N, M>(node, expr, GOTO);
  }

  //=
  template <unsigned int N, unsigned int M = N, typename Expr>
  static std::shared_ptr<crv_transition_fsm_state> non_consecutive_repeat(
      std::shared_ptr<crv_transition_fsm_state> node, Expr expr) {
    return addState<N, M>(node, expr, NON_CONSEC);
  }

 private:
  std::shared_ptr<crv_transition_fsm_state> succ; // successor state
  std::shared_ptr<crv_transition_fsm_state> prev; // previous state
  expression expr;
  int minHit;
  int maxHit;
  nodeType type;

  template <typename Expr>
  crv_transition_fsm_state(Expr nodeExpr, nodeType pType = CONSEC)
      : type(pType), succ(NULL), prev(NULL), expr(make_expression(nodeExpr)), minHit(1), maxHit(1) {}
};

/**
 \brief Interface for all types of bins.
 */
class crv_abstract_bin {
 public:
  /**
     * \brief Calculates if the bin was hit or not.
     * 
     * The calculation can be more or less complex based on the actual bin (plain
     * or transitional bin) behind it.
     * 
     * @param eval_ Evaluator object to use for calculation of a hit
     */
  virtual void calcHit(Evaluator &eval_) = 0;

  /** 
      * \brief Returns the minimal hit count.
      * \return The minimal hit count
      */
  unsigned hit_minimum() { return hit_minimum_; }

  /**
     * \brief Sets the minimal hit count to a new value.
     * \param min New hit minimum.
     */
  void hit_minimum(unsigned min) { hit_minimum_ = min; }

  /**
     * \brief Returns the current amount of hits.
     * \return Counter how often this bin has been hit.
     */
  unsigned hit_count() { return hit_count_; }

  /**
     * \brief Increases the hit counter.
     */
  void incrementHit() { ++hit_count_; }

  /**
     * \brief Checks if the bin is covered or not.
     * \return true if the bin is covered, false otherwise.
     */
  bool covered() { return hit_count_ >= hit_minimum_; }

 protected:
  crv_abstract_bin(unsigned phit_minimum_ = 1, unsigned phit_count_ = 0)
      : hit_minimum_(phit_minimum_), hit_count_(phit_count_) {}

 private:
  unsigned hit_minimum_;
  unsigned hit_count_;
};

/**
 * \ingroup newAPI
 * \brief A crv_bin represents a set of values for a variable within a coverpoint.
 * 
 * A crv_bin is hit if a value in the bin has been sampled.
 * A crv_bin is considered as covered if it has been hit at least a pre-defined number of times.
 */
class crv_bin : public crv_abstract_bin {
 public:
  /**
  * \brief Constructor with bin expression.
  * \param expr The expression that must be true to hit this bin.
  */
  template <typename Expr>
  crv_bin(Expr expr, unsigned int pHitMinimum = 1)
      : bin_expr_(make_expression(expr)) {
    hit_minimum(pHitMinimum);
  }

  /**
  * \brief Returns the expression this bin must met for a hit.
  * \return The bin expression
  */
  expression bin_expr() { return bin_expr_; }

  void calcHit(Evaluator &eval_) override {
    if (eval_.evaluate(bin_expr_) && eval_.result<bool>()) {
      incrementHit();
    }
  }

 private:
  expression bin_expr_;
};

/**
 * \ingroup newAPI
 * \brief A crv_transition_bin represents a transition for a variable within a coverpoint.
 * 
 * A crv_transition_bin is hit if its designed transition is fully matches by sampled values.
 * A crv_transition_bin is considered as covered if it has been hit at least a pre-defined number of times.
 */
class crv_transition_bin : public crv_abstract_bin {
 public:
  crv_transition_bin(std::shared_ptr<crv_transition_fsm_state> rootNode) : root(rootNode) {}

  virtual void calcHit(Evaluator &eval_) override {
    // First update all pending transitions, only carry those who can continue
    bool hit = false;
    std::vector<run> resultRuns;
    for (run current : runs) {
      checkRun(current, eval_, resultRuns, hit);
    }
    // Second add a new transition on start. If transition is only one constraint long, make a hit
    if (eval_.evaluate(root->expr) && eval_.result<bool>()) {
      checkRun(run(root, 0), eval_, resultRuns, hit);
    }

    // Third register only one hit per sample and save carried transitions
    if (hit) {
      incrementHit();
    }
    runs = resultRuns;
  }

 private:
  struct run {
    std::shared_ptr<crv_transition_fsm_state> state;
    int count;
    run(std::shared_ptr<crv_transition_fsm_state> pState, int pCount = 1) : state(pState), count(pCount){};
  };

  void checkRun(run current, Evaluator &eval_, std::vector<run> &resultRuns, bool &hit) {
    bool wasHit = eval_.evaluate(current.state->expr) && eval_.result<bool>();
    if (wasHit) {
      calcHit(eval_, current, resultRuns, hit);
    } else {
      if (current.state->type == crv_transition_fsm_state::CONSEC) {
        hardFail(eval_, current, resultRuns, hit);
      } else {
        softFail(eval_, current, resultRuns, hit);
      }
    }
  }

  inline void softFail(Evaluator &eval_, run &current, std::vector<run> &resultRuns, bool &hit) {
    // Just keep the current transition
    resultRuns.push_back(current);
  }

  inline void calcHit(Evaluator &eval_, run &current, std::vector<run> &resultRuns, bool &hit) {
    current.count++;
    bool belowMin = current.count < current.state->minHit;
    bool betweenMinMax = current.count >= current.state->minHit && current.count < current.state->maxHit;
    if (belowMin) {
      // Just increment counter
      resultRuns.push_back(run(current.state, current.count));
    } else if (betweenMinMax) {
      // Every hit between min and max continues the transition at the same state
      resultRuns.push_back(run(current.state, current.count));
      // But also can lead to the next state in the FSM
      if (!current.state->succ) {
        hit = true;
      } else {
        // Follow the successor route too
        resultRuns.push_back(run(current.state->succ, 0));
        // This adds the ability to find transitions also if they are
        // sooner finished than anticipated by maxHit.
      }
    } else {
      // When on max we must follow the successor, if exists. Else it is a hit
      if (!current.state->succ) {
        hit = true;
      } else {
        resultRuns.push_back(run(current.state->succ, 0));
      }
    }
  }

  inline void hardFail(Evaluator &eval_, run &current, std::vector<run> &resultRuns, bool &hit) {
    // Check if the previous state was an end of a non-consecutive repetition
    // This is because every node is allowed to fail, if the previous one was
    // a none-consective node (which forces fails after its end)
    if (current.state->prev && current.state->prev->type == crv_transition_fsm_state::NON_CONSEC) {
      // In that case, make sure we did not get another hit of the finished repetition
      bool hitAgain = eval_.evaluate(current.state->prev->expr) && eval_.result<bool>();
      if (!hitAgain) {
        // If we did not hit again, carry run to next sample
        resultRuns.push_back(current);
      }
    }
  }

  std::shared_ptr<crv_transition_fsm_state> root;
  std::vector<run> runs;
};

}  // end namespace crave
