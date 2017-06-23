#include "Hello.h"
#include <iostream>
#include <string.h>
#include <system_error>

using errc = std::errc;
using std::make_error_code;

namespace cppfuse
{

std::uintmax_t Hello::file_size(const fs::path& p)
{
    std::uintmax_t size = 0;
    if (p == hello_path)
    {
        size = hello_str.size();
    }
    return size;
}

fs::file_status Hello::symlink_status(const fs::path& path)
{

    auto result = fs::file_status{};
    if (path == "/")
    {
        result.type(fs::file_type::directory);
        auto perms = fs::perms::owner_exec | fs::perms::owner_read |
                     fs::perms::group_exec | fs::perms::group_read |
                     fs::perms::others_exec | fs::perms::others_read;
        result.permissions(perms);
    }
    else if (path == hello_path)
    {
        result.type(fs::file_type::regular);
        auto perms = fs::perms::owner_read | fs::perms::group_read |
                     fs::perms::others_read;
        result.permissions(perms);
    }
    else
    {
        auto ec = make_error_code(errc::no_such_file_or_directory);
        throw fs::filesystem_error("No such file or directory", ec);
    }
    return result;
}

std::vector<fs::path> Hello::read_directory(const fs::path& path)
{
    (void)path;
    std::vector<fs::path> result;
    result.push_back(fs::path("."));
    result.push_back(fs::path(".."));
    result.push_back(hello_path.filename());
    return result;
}

void Hello::open(const fs::path& path, int flags)
{
    if (path != hello_path)
    {
        auto ec = make_error_code(errc::no_such_file_or_directory);
        throw fs::filesystem_error("No such file or directory", ec);
    }
    else if ((flags & 3) != O_RDONLY)
    {
        auto ec = make_error_code(errc::permission_denied);
        throw fs::filesystem_error("Permission denied", ec);
    }
}

int Hello::read(const fs::path& path, string_view& buffer, uint64_t offset)
{
    int size = buffer.size();
    if (path != hello_path)
    {
        auto ec = make_error_code(errc::no_such_file_or_directory);
        throw fs::filesystem_error("No such file or directory", ec);
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

} // namespace cppfuse

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    auto mountpoint = cppfuse::fs::path("/mnt/hello");
    auto hello_fs = std::make_shared<cppfuse::Hello>();
    auto file_system = cppfuse::Fuse(hello_fs, mountpoint);
    file_system.mount();
    file_system.run();
    return 0;
}
