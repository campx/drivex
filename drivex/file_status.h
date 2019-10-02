#pragma once
#include <drivex/Permissions.h>
#include <drivex/file_type.h>

namespace lockblox {
namespace drivex {

class file_status {
 public:
  explicit file_status(file_type file_type,
                       drivex::permissions permissions =
                           drivex::permissions::perms_not_known) noexcept;

  explicit file_status(unsigned int mode);

  file_status() = default;
  file_status(const file_status&) = default;
  file_status& operator=(const file_status&) = default;
  ~file_status() = default;

  file_type type() const noexcept;
  drivex::permissions permissions() const noexcept;

  void type(file_type file_type) noexcept;
  void permissions(drivex::permissions permissions) noexcept;

  operator boost::filesystem::file_status() const;
  explicit operator unsigned int() const;

 private:
  file_type file_type_;
  drivex::permissions permissions_;
};
}  // namespace drivex
}  // namespace lockblox
