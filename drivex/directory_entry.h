#pragma once
#include <drivex/filesystem.h>

namespace lockblox {
namespace drivex {

class directory_entry {
 public: /** Constructors */
  directory_entry() = default;
  directory_entry(const directory_entry&) = default;

  directory_entry(directory_entry&&) = default;
  directory_entry(const filesystem& filesystem, Path p);
  ~directory_entry() = default;

 public: /** Modifiers */
  directory_entry& operator=(const directory_entry&) = default;

  directory_entry& operator=(directory_entry&&) = default;
  void assign(const Path& p);
  void replace_filename(const Path& p);

 public: /** Accessors */
  const Path& path() const noexcept;
  operator const Path&() const noexcept;
  file_status status() const;
  file_status symlink_status() const;

 public: /** Operators */
  bool operator<(const directory_entry& rhs) const noexcept;
  bool operator==(const directory_entry& rhs) const noexcept;
  bool operator!=(const directory_entry& rhs) const noexcept;
  bool operator<=(const directory_entry& rhs) const noexcept;
  bool operator>(const directory_entry& rhs) const noexcept;
  bool operator>=(const directory_entry& rhs) const noexcept;

 private: /** Data */
  const filesystem* filesystem_;
  Path p_;
};
}  // namespace drivex
}  // namespace lockblox
