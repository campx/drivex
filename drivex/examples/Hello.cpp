#include "Hello.h"
#include <iostream>

#if WIN32
auto mount_point = drivex::Path("H:\\");
#else
auto mount_point  = drivex::Path("/mnt/hello");
#endif

std::uintmax_t Hello::file_size(const drivex::Path& p) const
{
    std::uintmax_t size = 0;
    if (p == hello_path)
    {
        size = hello_str.size();
    }
    return size;
}

drivex::FileStatus Hello::symlink_status(const drivex::Path& path) const
{

    auto result = drivex::FileStatus{};
    if (path == "/")
    {
        result.type(drivex::FileType::directory);
        auto perms =
            drivex::Permissions::owner_exec | drivex::Permissions::owner_read |
            drivex::Permissions::group_exec | drivex::Permissions::group_read |
            drivex::Permissions::others_exec | drivex::Permissions::others_read;
        result.permissions(perms);
    }
    else if (path == hello_path)
    {
        result.type(drivex::FileType::regular);
        auto perms =
            drivex::Permissions::owner_read | drivex::Permissions::group_read |
            drivex::Permissions::others_read;
        result.permissions(perms);
    }
    else
    {
        throw drivex::Error(drivex::ErrorCode::no_such_file_or_directory);
    }
    return result;
}

std::vector<drivex::Path> Hello::read_directory(const drivex::Path& path) const
{
    (void) path;
    std::vector<drivex::Path> result;
    result.emplace_back(".");
    result.emplace_back("..");
    result.push_back(hello_path.filename());
    return result;
}

void Hello::open(const drivex::Path& path, int flags)
{
    if (path != hello_path)
    {
        throw drivex::Error(drivex::ErrorCode::no_such_file_or_directory);
    }
    else if ((flags & 3) != O_RDONLY)
    {
        throw drivex::Error(drivex::ErrorCode::permission_denied);
    }
}

int
Hello::read(const drivex::Path& path, drivex::string_view& buffer, uint64_t offset) const
{
    uint64_t size = buffer.size();
    if (path != hello_path)
    {
        throw drivex::Error(drivex::ErrorCode::no_such_file_or_directory);
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
        auto output = (void*) (buffer.data());
        memcpy(output, pos, size);
    }
    else
    {
        size = 0;
    }
    return size;
}

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;
    auto hello_fs = std::make_shared<Hello>();
    auto file_system = drivex::Fuse(hello_fs, mount_point);
    file_system.mount();
    file_system.run();
    return 0;
}
