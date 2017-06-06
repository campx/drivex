#include "Hello.h"

namespace cppfuse
{

Hello::Hello(fs::path mountpoint) : Fuse(mountpoint) {}

Stat Hello::getattr(const fs::path& path)
{
    Stat st;
    if (path == "/")
    {
        st.st_mode = S_IFDIR | 0755;
        st.st_nlink = 2;
    }
    else if (path == hello_path)
    {
        st.st_mode = S_IFREG | 0444;
        st.st_nlink = 1;
        st.st_size = hello_str.size();
    }
    else
    {
        auto ec = std::make_error_code(std::errc::no_such_file_or_directory);
        throw fs::filesystem_error("No such file or directory", ec);
    }

    return st;
}

std::vector<fs::path> Hello::readdir(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    std::vector<fs::path> result;
    result.push_back(fs::path("."));
    result.push_back(fs::path(".."));
    result.push_back(hello_path.filename());
    return result;
}

void Hello::open(const fs::path& path, FileInfo& info)
{
    if (path != hello_path)
    {
        auto ec = std::make_error_code(std::errc::no_such_file_or_directory);
        throw fs::filesystem_error("No such file or directory", ec);
    }
    else if ((info.flags & 3) != O_RDONLY)
    {
        auto ec = std::make_error_code(std::errc::permission_denied);
        throw fs::filesystem_error("Permission denied", ec);
    }
}

int Hello::read(const fs::path& path,
                string_view& buffer,
                uint64_t offset,
                FileInfo& info)
{
    (void)info;
    int size = buffer.size();
    if (path != hello_path)
    {
        auto ec = std::make_error_code(std::errc::no_such_file_or_directory);
        throw fs::filesystem_error("No such file or directory", ec);
    }
    auto len = hello_str.size();
    if (offset < len)
    {
        if (offset + size > len)
        {
            size = len - offset;
        }
        auto pos = hello_str.data();
        std::advance(pos, offset);
        auto output = (void*)(buffer.data());
        memcpy(output, pos, size);
    }
    return size;
}

} // namespace cppfuse

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    auto mountpoint = cppfuse::fs::path("/mnt/hello");
    auto file_system = cppfuse::Hello(mountpoint);
    file_system.mount();
    file_system.run();
    return 0;
}
