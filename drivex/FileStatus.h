#pragma once
#include <drivex/FileType.h>
#include <drivex/Permissions.h>

namespace lockblox {
namespace drivex {

class FileStatus {
 public:
  explicit FileStatus(
      FileType file_type,
      Permissions permissions = Permissions::perms_not_known) noexcept;

  explicit FileStatus(unsigned int mode);

  FileStatus() = default;
  FileStatus(const FileStatus&) = default;
  FileStatus& operator=(const FileStatus&) = default;
  ~FileStatus() = default;

  FileType type() const noexcept;
  Permissions permissions() const noexcept;

  void type(FileType file_type) noexcept;
  void permissions(Permissions permissions) noexcept;

  operator boost::filesystem::file_status() const;
  explicit operator unsigned int() const;

 private:
  FileType file_type_;
  Permissions permissions_;
};
}  // namespace drivex
}  // namespace lockblox
