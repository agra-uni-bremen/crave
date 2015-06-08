#include "CraveSettings.hpp"

namespace crave {

    explicit CraveSetting::CraveSetting(std::string const& filename)
    : Setting(filename),
    module_name_("crave"),
    backend_(),
    seed_(),
    BACKEND("backend"),
    SEED("seed") {
    }

    void CraveSetting::load_(const ptree& tree) {
        backend_ = tree.get(module_name_ + "." + BACKEND, "Boolector");
        seed_ = tree.get(module_name_ + "." + SEED, 42);
    }

    void CraveSetting::save_(ptree* tree) const {
        tree->put(module_name_ + "." + BACKEND, backend_);
        tree->put(module_name_ + "." + SEED, seed_);
    }

    std::string const& CraveSetting::get_backend() const {
        return backend_;
    }

    unsigned int CraveSetting::get_seed() const {
        return seed_;
    }
}
