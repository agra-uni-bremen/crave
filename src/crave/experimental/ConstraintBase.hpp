// Copyright 2012-2017 The CRAVE developers, University of Bremen, Germany. All rights reserved.
#pragma once

#include "Expression.hpp"
#include "Object.hpp"

namespace crave {

struct Generator;

/**
 * \ingroup newAPI
 * \brief Base class for crv_constraint.
 */
class crv_constraint_base : public crv_object {
 protected:
  crv_constraint_base();

 public:
  /**
   * \brief Get all expressions of this constraint.
   * \return The expressions of this constraint as a list.
   */
  virtual expression_list const& expr_list() const = 0;

  std::string obj_kind() const override final;

  /*!
   * \brief Activates this constraint.
   *
   * An activated constraint will be considered during randomization.
   */
  void activate();

  /*!
   * \brief Deactivates this constraint.
   *
   * A deactivated constraint will not be considered during randomization.
   */
  void deactivate();

  /*!
   * \brief Returns if the constraint is active.
   * \return true if it is active, false otherwise.
   */
  bool active() const;

  /**
   * \brief Checks if constraint is a soft constraint.
   * \return True if this constraint is a soft constraint, false otherwise
   */
  virtual bool soft() const = 0;

  /**
   * \brief Internal implementation details.
   */
  void recursive_build(Generator& gen) const override;

 protected:
  bool active_;
};
}  // namespace crave
