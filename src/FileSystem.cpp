#include <cpp-fuse/FileSystem.h>
#include <iostream>
#include <system_error>

using errc = std::errc;
using std::make_error_code;

namespace cppfuse
{

fs::path FileSystem::absolute(const fs::path& path) const noexcept
{
    auto output = fs::path();
    if (path.is_relative())
    {
        output /= current_path();
    }
    for (auto dir : path)
    {
        if (dir == "..")
        {
            output = parent_path(output);
        }
        else if (dir != ".")
        {
            output /= dir;
        }
    }
    return output;
}

fs::path FileSystem::parent_path(const fs::path& path)
{
    return path == "/" ? path : path.parent_path();
}

const fs::path& FileSystem::current_path() const noexcept
{
    return current_path_;
}

void FileSystem::unsupported() const
{
    auto ec = make_error_code(errc::function_not_supported);
    throw fs::filesystem_error("operation not supported by filesystem", ec);
}

std::uintmax_t FileSystem::file_size(const fs::path& path)
{
    (void)path;
    unsupported();
    return 0;
}

fs::file_status FileSystem::status(const fs::path& path)
{
    (void)path;
    unsupported();
    return fs::file_status{};
}

fs::file_status FileSystem::symlink_status(const fs::path& path)
{
    return status(path);
}

fs::path FileSystem::read_symlink(const fs::path& path)
{
    (void)path;
    unsupported();
    return fs::path();
}

void FileSystem::create_directory(const fs::path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::unlink(const fs::path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::rmdir(const fs::path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::symlink(const fs::path& target, const fs::path& link)
{
    (void)target;
    (void)link;
    unsupported();
}

void FileSystem::rename(const fs::path& from, const fs::path& to)
{
    (void)from;
    (void)to;
    unsupported();
}

void FileSystem::link(const fs::path& from, const fs::path& to)
{
    (void)from;
    (void)to;
    unsupported();
}

void FileSystem::permissions(const fs::path& path, fs::perms permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void FileSystem::chown(const fs::path& path,
                       uint32_t user_id,
                       uint32_t group_id)
{
    (void)path;
    (void)user_id;
    (void)group_id;
    unsupported();
}

void FileSystem::truncate(const fs::path& path, uint64_t offset)
{
    (void)path;
    (void)offset;
    unsupported();
}

void FileSystem::open(const fs::path& path, int flags)
{
    (void)path;
    (void)flags;
    unsupported();
}

int FileSystem::read(const fs::path& path,
                     string_view& buffer,
                     uint64_t offset)
{
    (void)path;
    (void)buffer;
    (void)offset;
    unsupported();
    return 0;
}

int FileSystem::write(const fs::path& path,
                      const string_view& buffer,
                      uint64_t offset)
{
    (void)path;
    (void)buffer;
    (void)offset;
    unsupported();
    return 0;
}

void FileSystem::flush(const fs::path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::release(const fs::path& path, int flags)
{
    (void)path;
    (void)flags;
    unsupported();
}

void FileSystem::fsync(const fs::path& path, int fd)
{
    (void)path;
    (void)fd;
    unsupported();
}

void FileSystem::setxattr(const fs::path& path,
                          std::pair<std::string, string_view> attribute,
                          int flags)
{
    (void)path;
    (void)attribute;
    (void)flags;
    unsupported();
}

std::pair<std::string, string_view>
FileSystem::getxattr(const fs::path& path, const std::string& name)
{
    (void)path;
    (void)name;
    unsupported();
    return std::pair<std::string, string_view>{};
}

std::vector<std::string> FileSystem::listxattr(const fs::path& path)
{
    (void)path;
    unsupported();
    return std::vector<std::string>{};
}

void FileSystem::removexattr(const fs::path& path, const std::string& name)
{
    (void)path;
    (void)name;
    unsupported();
}

std::vector<fs::path> FileSystem::read_directory(const fs::path& path)
{
    (void)path;
    unsupported();
    return std::vector<fs::path>{};
}

void FileSystem::fsyncdir(const fs::path& path, int datasync)
{
    (void)path;
    (void)datasync;
    unsupported();
}

void FileSystem::access(const fs::path& path, const fs::perms& permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void FileSystem::create_file(const fs::path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::lock(const fs::path& path, int command)
{
    (void)path;
    (void)command;
    unsupported();
}

std::time_t FileSystem::last_read_time(const fs::path& path)
{
    (void)path;
    unsupported();
    return std::time_t{};
}

void FileSystem::last_read_time(const fs::path& path, std::time_t new_time)
{
    (void)path;
    (void)new_time;
    unsupported();
}

std::time_t FileSystem::last_write_time(const fs::path& path)
{
    (void)path;
    unsupported();
    return std::time_t{};
}

void FileSystem::last_write_time(const fs::path& path, std::time_t new_time)
{
    (void)path;
    (void)new_time;
    unsupported();
}

uint64_t FileSystem::bmap(const fs::path& path, size_t blocksize)
{
    (void)path;
    (void)blocksize;
    unsupported();
    return 0;
}

void FileSystem::ioctl(
    const fs::path& path, int cmd, void* arg, unsigned int flags, void* data)
{
    (void)path;
    (void)cmd;
    (void)arg;
    (void)flags;
    (void)data;
    unsupported();
}

void FileSystem::fallocate(const fs::path& path,
                           int mode,
                           uint64_t offset,
                           uint64_t length)
{
    (void)path;
    (void)mode;
    (void)offset;
    (void)length;
    unsupported();
}

} // namespace cppfuse
