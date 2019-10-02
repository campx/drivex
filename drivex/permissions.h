#pragma once
#include <boost/filesystem/operations.hpp>

namespace lockblox {
namespace drivex {

enum class permissions : unsigned int {
  owner_read = boost::filesystem::perms::owner_read,
  owner_write = boost::filesystem::perms::owner_write,
  owner_exec = boost::filesystem::perms::owner_exe,
  owner_all = boost::filesystem::perms::owner_all,

  group_read = boost::filesystem::perms::group_read,
  group_write = boost::filesystem::perms::group_write,
  group_exec = boost::filesystem::perms::group_exe,
  group_all = boost::filesystem::perms::group_all,

  others_read = boost::filesystem::perms::others_read,
  others_write = boost::filesystem::perms::others_write,
  others_exec = boost::filesystem::perms::others_exe,
  others_all = boost::filesystem::perms::others_all,

  perms_not_known = boost::filesystem::perms::perms_not_known
};

permissions operator|(permissions lhs, permissions rhs);
permissions operator&(permissions lhs, permissions rhs);

}  // namespace drivex
}  // namespace lockblox
