#include "../crave/ir/ConstraintManager.hpp"


namespace crave {

    ConstraintManager::ConstraintManager() : changed_(false) {
        static unsigned ID = 0;
        id_ = ++ID;
    }
}