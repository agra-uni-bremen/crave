#include "../crave/utils/CraveSettings.hpp"

namespace crave {

CraveSetting::CraveSetting(std::string const& filename)
    : Setting(filename), module_name_("crave"), backend_(), specified_seed_(), used_seed_(), BACKEND("backend"), SEED("seed"), LASTSEED("lastseed") {}

void CraveSetting::load_(const ptree& tree) {
  backend_ = tree.get(module_name_ + "." + BACKEND, "auto");
  specified_seed_ = tree.get(module_name_ + "." + SEED, 0);
}

void CraveSetting::save_(ptree* tree) const {
  tree->put(module_name_ + "." + BACKEND, backend_);
  tree->put(module_name_ + "." + SEED, specified_seed_);
  tree->put(module_name_ + "." + LASTSEED, used_seed_);
}

std::string const& CraveSetting::get_backend() const { return backend_; }

unsigned int CraveSetting::get_specified_seed() const { return specified_seed_; }

void CraveSetting::set_used_seed(unsigned int seed) { used_seed_ = seed; }

}
