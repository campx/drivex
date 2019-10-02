#include <drivex/Permissions.h>

namespace lockblox {
namespace drivex {

permissions operator|(permissions lhs, permissions rhs) {
  return static_cast<permissions>(static_cast<unsigned int>(lhs) |
                                  static_cast<unsigned int>(rhs));
}

permissions operator&(permissions lhs, permissions rhs) {
  return static_cast<permissions>(static_cast<unsigned int>(lhs) &
                                  static_cast<unsigned int>(rhs));
}

}  // namespace drivex
}  // namespace lockblox
