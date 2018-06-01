#include "Hello.h"
#include <iostream>
#include <string.h>
#include <system_error>

std::uintmax_t Hello::file_size(const filex::Path& p) const
{
    std::uintmax_t size = 0;
    if (p == hello_path)
    {
        size = hello_str.size();
    }
    return size;
}

filex::FileStatus Hello::symlink_status(const filex::Path& path) const
{

    auto result = filex::FileStatus{};
    if (path == "/")
    {
        result.type(filex::FileType::directory);
        auto perms =
            filex::Permissions::owner_exec | filex::Permissions::owner_read |
            filex::Permissions::group_exec | filex::Permissions::group_read |
            filex::Permissions::others_exec | filex::Permissions::others_read;
        result.permissions(perms);
    }
    else if (path == hello_path)
    {
        result.type(filex::FileType::regular);
        auto perms = filex::Permissions::owner_read |
                     filex::Permissions::group_read |
                     filex::Permissions::others_read;
        result.permissions(perms);
    }
    else
    {
        throw filex::Error(filex::ErrorCode::no_such_file_or_directory);
    }
    return result;
}

std::vector<filex::Path> Hello::read_directory(const filex::Path& path) const
{
    (void)path;
    std::vector<filex::Path> result;
    result.push_back(filex::Path("."));
    result.push_back(filex::Path(".."));
    result.push_back(hello_path.filename());
    return result;
}

void Hello::open(const filex::Path& path, int flags)
{
    if (path != hello_path)
    {
        throw filex::Error(filex::ErrorCode::no_such_file_or_directory);
    }
    else if ((flags & 3) != O_RDONLY)
    {
        throw filex::Error(filex::ErrorCode::permission_denied);
    }
}

int Hello::read(const filex::Path& path,
                string_view& buffer,
                uint64_t offset) const
{
    int size = buffer.size();
    if (path != hello_path)
    {
        throw filex::Error(filex::ErrorCode::no_such_file_or_directory);
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

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    auto mountpoint = filex::Path("/mnt/hello");
    auto hello_fs = std::make_shared<Hello>();
    auto file_system = filex::Fuse(hello_fs, mountpoint);
    file_system.mount();
    file_system.run();
    return 0;
}
