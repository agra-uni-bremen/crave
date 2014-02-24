#include <ctime>

#include <boost/filesystem.hpp>

#include <glog/logging.h>

#include "../crave/expression/FactoryMetaSMT.hpp"
#include "../crave/ConstrainedRandom.hpp"
#include "../crave/utils/Settings.hpp"

namespace crave {

  int new_var_id() { static int var_ID=0; return ++var_ID;}

  int new_constraint_id() { static int constraint_ID=0; return ++constraint_ID; }

  unsigned int default_rand_vec_size() { return 5; }

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

    CraveSetting cSettings(cfg_file);
    cSettings.load();

    set_global_seed(cSettings.get_seed());
    set_solver_backend(cSettings.get_backend());
    set_config_file_name(cfg_file);

    // initalize glog
    LoggerSetting settings(cfg_file);

    settings.load();
    FLAGS_log_dir = settings.dirname();
    FLAGS_max_log_size = settings.filesize();
    FLAGS_logtostderr = false;
    FLAGS_logbufsecs = 0;

    namespace fs = boost::filesystem;
    fs::path fs_log_dir(settings.dirname());
    if (!fs::exists(fs_log_dir))
      fs::create_directory(fs_log_dir);

    static bool initialized = false;
    if (!initialized) {
      google::InitGoogleLogging(settings.filename().c_str());
      initialized = true;
    }

    cSettings.save();
    settings.save();
  }

  void init(std::string const& solver_type, std::string const& cfg_file) {
    // set solver backend
    FactoryMetaSMT::setSolverType(solver_type);

    init(cfg_file);
  }

  std::ostream& print_dot_graph_(std::ostream& os, ConstraintList& constraints)  {
    os << "digraph AST {" << std::endl;
    ToDotVisitor visitor(os);

    BOOST_FOREACH ( ConstraintPtr c , constraints ) {
      long a = reinterpret_cast<long>(&*c);
      long b = reinterpret_cast<long>(&(*c->get_expression()));
      os << "\t" << a << " [label=\"" << c->get_name() << (c->is_soft()?" soft":"") << (!c->is_enabled()?" disabled":"") << "\"]" << std::endl;
      os << "\t" << a << " -> " << b << std::endl; 
      c->get_expression()->visit(visitor);
    }

    os << "}" << std::endl;
    return os;
  }

} // namespace crave

// vim: tabstop=2 shiftwidth=2 expandtab
