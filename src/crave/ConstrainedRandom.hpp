// Copyright 2012-2016 The CRAVE developers, University of Bremen, Germany. All rights reserved.//

#pragma once

#include <boost/preprocessor.hpp>
#include <set>
#include <string>
#include <vector>

#include "backend/Generator.hpp"
#include "frontend/RandomBase.hpp"
#include "ir/UserExpression.hpp"
#include "utils/Logging.hpp"

/*!
 * \brief Default namespace of crave.
 *
 * Both the old and new API are in the namespace crave.
 */
namespace crave {
/*!
 * \ingroup setting
 * \brief Sets global random seed.
 *
 * This function can be used for repeatable randomization.
 * If a non-zero seed is set, CRAVE will generate the same set of random values in every run.
 * Otherwise, CRAVE will use a random seed.
 *
 * \param s A positive random seed. 0 indicates a random seed.
 */
void set_global_seed(unsigned int s);
/*!
 * \ingroup setting
 * \brief Sets the solver to use.
 *
 * This function can be used to specify the solver backend by its name (e.g. Boolector, Z3, etc.).
 * The string "auto" is given or the specified backend is not available, CRAVE will choose the backend automatically.
 *
 * \param type Name of the solver, "auto" to let CRAVE decide.
 */
void set_solver_backend(std::string const&);

/*!
 * \ingroup setting
 * \brief Gets the name of the config file.
 *
 * Returns the name of the current config file.
 * The name can be changed via crave::set_config_file_name(std::string const&).
 * Default value is "crave.cfg".
 */
std::string const& get_config_file_name();
/*!
 * \ingroup setting
 * \brief Sets the name of the config file.
 *
 * Sets the name of the config file to be used.
 * The name can be retrieved via crave::get_config_file_name().
 *
 * \param str New file name
 */
void set_config_file_name(std::string const&);

/*!
 * \ingroup setting
 * \brief Initializes CRAVE.
 *
 * Init without parameter uses the value returned by crave::get_config_file_name().
 * For details see crave::init(std::string const& cfg_file).
 */
void init();
/*!
 * \ingroup setting
 * \brief Initializes CRAVE with a given config file.
 *
 * <p>
 * Reads configuration for CRAVE and the logger from a config file.
 * The config file is a XML file containing informations about the backend to be used and the seed for randomization.
 * 0 indicates a random seed.
 * </p><p>
 * Also the config file contains settings for the logger.
 * The path to the log file, its maximum size and a log severity level between 0..3 can be set.
 * The severity levels are INFO, WARNING, ERROR, and FATAL (0, 1, 2, and 3, respectively).
 * The amount of log messages decreases with increasing severity level.
 * </p><p>
 * If the file does not exist, it will be created with a default config of CRAVE.
 */
void init(std::string const&);

/*!
 * \ingroup operators
 * \brief Predefined placeholder for the iteration variable used in crave::foreach.
 *
 * crave::foreach is used to define constraints over vectors.
 * This predefined placeholder _i can be used with the operator [] to refer to the current element of the vector.
 */
extern placeholder _i;

/*!
 * \ingroup oldAPI
 * \deprecated It is recommended to use the \ref newAPI "new API". The equivalent in the new API is \ref
 * crave::crv_sequence_item \brief Base class for randomizable objects.
 *
 * Every class whose objects are to be randomized by CRAVE must extend this class.
 * The inheritance includes basic constrained randomization as well as constraint management capabilities.
 */
class rand_obj : public rand_obj_base {
 public:
  explicit rand_obj(rand_obj* parent = 0);

  virtual bool next();

  virtual bool next_cov();

  virtual void gather_values(std::vector<int64_t>* ch);

  virtual void add_base_child(rand_base* rb);

  void request_rebuild();

  void add_obj_child(rand_obj* ro);

  bool enable_constraint(std::string name);

  bool disable_constraint(std::string name);

  bool is_constraint_enabled(std::string name);

  std::ostream& print_dot_graph(std::ostream&, bool);
  void display_constraints();

 protected:
  bool gen_base_children();

  void gather_constraints(Generator* gen);

 public:
  Generator constraint;

 protected:
  std::vector<rand_base*> baseChildren_;
  std::vector<rand_obj*> objChildren_;
  rand_obj* parent_;
  bool rebuild_;
};

}  // end namespace crave

#define __CRAVE_INSERT(s, DATA, ELEM) DATA.insert(ELEM);

/*!
 * \addtogroup oldAPI
 *  @{
 * \def CRAVE_ENUM(name, ...)
 * \deprecated It is recommended to use the \ref newAPI "new API". The equivalent in the new API is \ref
 * CRAVE_BETTER_ENUM \brief Defines a randomizable enum for CRAVE.
 *
 * Any enum to be randomized should be defined by this macro.
 * "name" is the name of your already defined enum.
 * Then, all possible values of the enum should follow.
 * Internally, the enum will be presented with a randv<int>.
 */
#define CRAVE_ENUM(name, ...)                                                              \
  namespace crave {                                                                        \
  template <>                                                                              \
  struct randv<name> : randv<int> {                                                        \
    explicit randv(rand_obj* parent) : randv<int>(parent) {                                \
      if (parent == 0) {                                                                   \
        throw std::runtime_error("randv<enum> must be owned by an instance of rand_obj."); \
      }                                                                                    \
      std::set<int> s;                                                                     \
      BOOST_PP_SEQ_FOR_EACH(__CRAVE_INSERT, s, __VA_ARGS__);                               \
      parent->constraint(inside(var, s));                                                  \
    }                                                                                      \
  };                                                                                       \
  }  // namespace crave

/*! @}*/
