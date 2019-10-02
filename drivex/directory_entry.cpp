#include <drivex/directory_entry.h>

namespace lockblox {
namespace drivex {

directory_entry::directory_entry(const filesystem& filesystem, Path p)
    : filesystem_(&filesystem), p_(std::move(p)) {}

void directory_entry::assign(const Path& p) { p_ = p; }

void directory_entry::replace_filename(const Path& p) {
  p_ = p_.parent_path() / p;
}

const Path& directory_entry::path() const noexcept { return p_; }

directory_entry::operator const Path&() const noexcept { return p_; }

file_status directory_entry::status() const { return filesystem_->status(p_); }

file_status directory_entry::symlink_status() const {
  return filesystem_->symlink_status(p_);
}

bool directory_entry::operator<(const directory_entry& rhs) const noexcept {
  return p_ < rhs.p_;
}

bool directory_entry::operator==(const directory_entry& rhs) const noexcept {
  return p_ == rhs.p_;
}

bool directory_entry::operator!=(const directory_entry& rhs) const noexcept {
  return p_ != rhs.p_;
}

bool directory_entry::operator<=(const directory_entry& rhs) const noexcept {
  return p_ <= rhs.p_;
}

bool directory_entry::operator>(const directory_entry& rhs) const noexcept {
  return p_ > rhs.p_;
}

bool directory_entry::operator>=(const directory_entry& rhs) const noexcept {
  return p_ >= rhs.p_;
}
}  // namespace drivex
}  // namespace lockblox
