#include <drivex/DirectoryEntry.h>
namespace drivex
{

DirectoryEntry::DirectoryEntry(const FileSystem& filesystem, Path p)
    : filesystem_(&filesystem), p_(std::move(p))
{
}

void DirectoryEntry::assign(const Path& p) { p_ = p; }

void DirectoryEntry::replace_filename(const Path& p)
{
    p_ = p_.parent_path() / p;
}

const Path& DirectoryEntry::path() const noexcept { return p_; }

DirectoryEntry::operator const Path&() const noexcept { return p_; }

FileStatus DirectoryEntry::status() const { return filesystem_->status(p_); }

FileStatus DirectoryEntry::symlink_status() const
{
    return filesystem_->symlink_status(p_);
}

bool DirectoryEntry::operator<(const DirectoryEntry& rhs) const noexcept
{
    return p_ < rhs.p_;
}

bool DirectoryEntry::operator==(const DirectoryEntry& rhs) const noexcept
{
    return p_ == rhs.p_;
}

bool DirectoryEntry::operator!=(const DirectoryEntry& rhs) const noexcept
{
    return p_ != rhs.p_;
}

bool DirectoryEntry::operator<=(const DirectoryEntry& rhs) const noexcept
{
    return p_ <= rhs.p_;
}

bool DirectoryEntry::operator>(const DirectoryEntry& rhs) const noexcept
{
    return p_ > rhs.p_;
}

bool DirectoryEntry::operator>=(const DirectoryEntry& rhs) const noexcept
{
    return p_ >= rhs.p_;
}

} // namespace drivex
