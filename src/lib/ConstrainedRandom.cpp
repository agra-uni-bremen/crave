#include <ctime>

#include <boost/filesystem.hpp>

#include <glog/logging.h>

#include "../crave/expression/FactoryMetaSMT.hpp"
#include "../crave/ConstrainedRandom.hpp"
#include "../crave/utils/Settings.hpp"

namespace crave {

  int new_var_id() { static int var_ID=0; return ++var_ID;}

  int new_constraint_id() { static int constraint_ID=0; return ++constraint_ID; }

  boost::mt19937 rng(std::time(0));

  struct random_bit_gen {
    random_bit_gen() : rnd(0, 1) { }

    bool operator() () {
      return rnd(rng);
    }

    boost::uniform_int<unsigned short> rnd;
  };

  boost::function0<bool> random_bit = random_bit_gen();

  void set_global_seed(unsigned int s) { rng.seed(s); };

  void set_solver_backend(std::string const& type) {
    FactoryMetaSMT::setSolverType(type);
  }

  std::string config_file_name = "crave.cfg";
  std::string const& get_config_file_name() { return config_file_name; }
  void set_config_file_name(std::string const& str) { config_file_name = str; }

  void init() {
    init(get_config_file_name());
  }

  void init(std::string const& cfg_file) {

    // set global seed
    set_global_seed(0);
    set_config_file_name(cfg_file);

    // initalize glog
    LoggerSetting settings(cfg_file);

    settings.load();
    fLS::FLAGS_log_dir = settings.dirname();
    fLI::FLAGS_max_log_size = settings.filesize();
    fLB::FLAGS_logtostderr = false;

    namespace fs = boost::filesystem;
    fs::path fs_log_dir(settings.dirname());
    if (!fs::exists(fs_log_dir))
      fs::create_directory(fs_log_dir);

    static bool initialized = false;
    if (!initialized) {
      google::InitGoogleLogging(settings.filename().c_str());
      initialized = true;
    }

    settings.save();
  }

  void init(std::string const& solver_type, std::string const& cfg_file) {
    // set solver backend
    FactoryMetaSMT::setSolverType(solver_type);

    init(cfg_file);
  }

} // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
