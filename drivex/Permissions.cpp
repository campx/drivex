#include <drivex/Permissions.h>

namespace lockblox {
namespace drivex {

Permissions operator|(Permissions lhs, Permissions rhs) {
  return static_cast<Permissions>(static_cast<unsigned int>(lhs) |
                                  static_cast<unsigned int>(rhs));
}
}  // namespace drivex
}  // namespace lockblox
