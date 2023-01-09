#include "Utils_geometry.hpp"


std::ostream &operator<<(std::ostream &os, fPoint const &p) {
    return os << "{" << p.x << "," << p.y << "}";
}
std::ostream &operator<<(std::ostream &os, fSize const &p) {
    return os << "{" << p.w << "," << p.h << "}";
}
