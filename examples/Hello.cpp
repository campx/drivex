#include "Hello.h"
#include <iostream>
#include <string.h>
#include <system_error>

using errc = std::errc;
using std::make_error_code;

namespace x
{

std::uintmax_t Hello::file_size(const filesystem::Path& p)
{
    std::uintmax_t size = 0;
    if (p == hello_path)
    {
        size = hello_str.size();
    }
    return size;
}

filesystem::FileStatus Hello::symlink_status(const filesystem::Path& path)
{

    auto result = filesystem::FileStatus{};
    if (path == "/")
    {
        result.type(filesystem::FileType::directory);
        auto perms = filesystem::Permissions::owner_exec |
                     filesystem::Permissions::owner_read |
                     filesystem::Permissions::group_exec |
                     filesystem::Permissions::group_read |
                     filesystem::Permissions::others_exec |
                     filesystem::Permissions::others_read;
        result.permissions(perms);
    }
    else if (path == hello_path)
    {
        result.type(filesystem::FileType::regular);
        auto perms = filesystem::Permissions::owner_read |
                     filesystem::Permissions::group_read |
                     filesystem::Permissions::others_read;
        result.permissions(perms);
    }
    else
    {
        auto ec = make_error_code(errc::no_such_file_or_directory);
        throw filesystem::Error("No such file or directory", ec);
    }
    return result;
}

std::vector<filesystem::Path>
Hello::read_directory(const filesystem::Path& path)
{
    (void)path;
    std::vector<filesystem::Path> result;
    result.push_back(filesystem::Path("."));
    result.push_back(filesystem::Path(".."));
    result.push_back(hello_path.filename());
    return result;
}

void Hello::open(const filesystem::Path& path, int flags)
{
    if (path != hello_path)
    {
        auto ec = make_error_code(errc::no_such_file_or_directory);
        throw filesystem::Error("No such file or directory", ec);
    }
    else if ((flags & 3) != O_RDONLY)
    {
        auto ec = make_error_code(errc::permission_denied);
        throw filesystem::Error("Permission denied", ec);
    }
}

int Hello::read(const filesystem::Path& path,
                string_view& buffer,
                uint64_t offset)
{
    int size = buffer.size();
    if (path != hello_path)
    {
        auto ec = make_error_code(errc::no_such_file_or_directory);
        throw filesystem::Error("No such file or directory", ec);
    }
    auto len = hello_str.size();
    if (offset < len)
    {
        if (offset + buffer.size() > len)
        {
            size = len - offset;
        }
        auto pos = hello_str.data();
        std::advance(pos, offset);
        auto output = (void*)(buffer.data());
        memcpy(output, pos, size);
    }
    else
    {
        size = 0;
    }
    return size;
}

} // namespace x

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    auto mountpoint = x::filesystem::Path("/mnt/hello");
    auto hello_fs = std::make_shared<x::Hello>();
    auto file_system = x::fuse::Fuse(hello_fs, mountpoint);
    file_system.mount();
    file_system.run();
    return 0;
}
