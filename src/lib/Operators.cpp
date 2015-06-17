// Copyright 2014 The CRAVE developers. All rights reserved.//

#include "../crave/frontend/Operators.hpp"
#include <ostream>

namespace crave {

    template <typename OUT>
    OUT& operator<<(OUT& out, placeholder_tag const& tag) {
        out << "placeholder<" << tag.id << ">";
        return out;
    }
    template std::ostream& operator<<(std::ostream& out, placeholder_tag const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_inside const& tag) {
        return out << "inside";
    }
    template std::ostream& operator<<(std::ostream& out, operator_inside const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_dist const& tag) {
        return out << "dist";
    }
    template std::ostream& operator<<(std::ostream& out, operator_dist const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_if_then const& tag) {
        return out << "if_then";
    }
    template std::ostream& operator<<(std::ostream& out, operator_if_then const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_if_then_else const& tag) {
        return out << static_cast<operator_if_then> (tag) << "_else";
    }
    template std::ostream& operator<<(std::ostream& out, operator_if_then_else const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_foreach const& tag) {
        return out << "foreach";
    }
    template std::ostream& operator<<(std::ostream& out, operator_foreach const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_unique const& tag) {
        return out << "unique";
    }
    template std::ostream& operator<<(std::ostream& out, operator_unique const& tag);

    template <typename OUT>
    OUT& operator<<(OUT& out, operator_bitslice const& tag) {
        return out << "bitslice";
    }
    template std::ostream& operator<<(std::ostream& out, operator_bitslice const& tag);
}