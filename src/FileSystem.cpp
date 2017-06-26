#include <fusex/FileSystem.h>
#include <iostream>
#include <system_error>

using errc = std::errc;
using std::make_error_code;

namespace x
{

namespace filesystem
{

Path FileSystem::absolute(const Path& path) const noexcept
{
    auto output = Path();
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

Path FileSystem::parent_path(const Path& path)
{
    return path == "/" ? path : path.parent_path();
}

const Path& FileSystem::current_path() const noexcept { return current_path_; }

void FileSystem::unsupported() const
{
    auto ec = make_error_code(errc::function_not_supported);
    throw Error("operation not supported by filesystem", ec);
}

std::uintmax_t FileSystem::file_size(const Path& path)
{
    (void)path;
    unsupported();
    return 0;
}

FileStatus FileSystem::status(const Path& path)
{
    (void)path;
    unsupported();
    return FileStatus{};
}

FileStatus FileSystem::symlink_status(const Path& path)
{
    return status(path);
}

Path FileSystem::read_symlink(const Path& path)
{
    (void)path;
    unsupported();
    return Path();
}

void FileSystem::create_directory(const Path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::unlink(const Path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::rmdir(const Path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::symlink(const Path& target, const Path& link)
{
    (void)target;
    (void)link;
    unsupported();
}

void FileSystem::rename(const Path& from, const Path& to)
{
    (void)from;
    (void)to;
    unsupported();
}

void FileSystem::link(const Path& from, const Path& to)
{
    (void)from;
    (void)to;
    unsupported();
}

void FileSystem::permissions(const Path& path, Permissions permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void FileSystem::chown(const Path& path, uint32_t user_id, uint32_t group_id)
{
    (void)path;
    (void)user_id;
    (void)group_id;
    unsupported();
}

void FileSystem::truncate(const Path& path, uint64_t offset)
{
    (void)path;
    (void)offset;
    unsupported();
}

void FileSystem::open(const Path& path, int flags)
{
    (void)path;
    (void)flags;
    unsupported();
}

int FileSystem::read(const Path& path, string_view& buffer, uint64_t offset)
{
    (void)path;
    (void)buffer;
    (void)offset;
    unsupported();
    return 0;
}

int FileSystem::write(const Path& path,
                      const string_view& buffer,
                      uint64_t offset)
{
    (void)path;
    (void)buffer;
    (void)offset;
    unsupported();
    return 0;
}

void FileSystem::flush(const Path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::release(const Path& path, int flags)
{
    (void)path;
    (void)flags;
    unsupported();
}

void FileSystem::fsync(const Path& path, int fd)
{
    (void)path;
    (void)fd;
    unsupported();
}

void FileSystem::setxattr(const Path& path,
                          std::pair<std::string, string_view> attribute,
                          int flags)
{
    (void)path;
    (void)attribute;
    (void)flags;
    unsupported();
}

std::pair<std::string, string_view>
FileSystem::getxattr(const Path& path, const std::string& name)
{
    (void)path;
    (void)name;
    unsupported();
    return std::pair<std::string, string_view>{};
}

std::vector<std::string> FileSystem::listxattr(const Path& path)
{
    (void)path;
    unsupported();
    return std::vector<std::string>{};
}

void FileSystem::removexattr(const Path& path, const std::string& name)
{
    (void)path;
    (void)name;
    unsupported();
}

std::vector<Path> FileSystem::read_directory(const Path& path)
{
    (void)path;
    unsupported();
    return std::vector<Path>{};
}

void FileSystem::fsyncdir(const Path& path, int datasync)
{
    (void)path;
    (void)datasync;
    unsupported();
}

void FileSystem::access(const Path& path, const Permissions& permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void FileSystem::create_file(const Path& path)
{
    (void)path;
    unsupported();
}

void FileSystem::lock(const Path& path, int command)
{
    (void)path;
    (void)command;
    unsupported();
}

std::time_t FileSystem::last_read_time(const Path& path)
{
    (void)path;
    unsupported();
    return std::time_t{};
}

void FileSystem::last_read_time(const Path& path, std::time_t new_time)
{
    (void)path;
    (void)new_time;
    unsupported();
}

std::time_t FileSystem::last_write_time(const Path& path)
{
    (void)path;
    unsupported();
    return std::time_t{};
}

void FileSystem::last_write_time(const Path& path, std::time_t new_time)
{
    (void)path;
    (void)new_time;
    unsupported();
}

uint64_t FileSystem::bmap(const Path& path, size_t blocksize)
{
    (void)path;
    (void)blocksize;
    unsupported();
    return 0;
}

void FileSystem::ioctl(
    const Path& path, int cmd, void* arg, unsigned int flags, void* data)
{
    (void)path;
    (void)cmd;
    (void)arg;
    (void)flags;
    (void)data;
    unsupported();
}

void FileSystem::fallocate(const Path& path,
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

} // namespace filesystem

} // namespace x
