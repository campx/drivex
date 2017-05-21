#include "Hello.h"

namespace cppfuse
{

Hello::Hello(fs::path mountpoint) : Fuse(mountpoint) {}

int Hello::getattr(const char* path, struct stat* stbuf)
{
    int res = 0;

    memset(stbuf, 0, sizeof(struct stat));
    if (strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if (path == hello_path)
    {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = hello_str.size();
    }
    else
    {
        res = -ENOENT;
    }

    return res;
}

int Hello::readdir(const char* path,
                   void* buf,
                   fuse_fill_dir_t filler,
                   off_t offset,
                   struct fuse_file_info* fi)
{
    (void)offset;
    (void)fi;

    if (strcmp(path, "/") != 0)
    {
        return -ENOENT;
    }

    auto pos = hello_path.data();
    std::advance(pos, 1);
    filler(buf, ".", nullptr, 0);
    filler(buf, "..", nullptr, 0);
    filler(buf, pos, nullptr, 0);

    return 0;
}

int Hello::open(const char* path, struct fuse_file_info* fi)
{
    if (path != hello_path)
    {
        return -ENOENT;
    }

    if ((fi->flags & 3) != O_RDONLY)
    {
        return -EACCES;
    }

    return 0;
}

int Hello::read(const char* path,
                char* buf,
                size_t size,
                off_t offset,
                struct fuse_file_info* fi)
{
    off_t len;
    (void)fi;
    if (path != hello_path)
    {
        return -ENOENT;
    }

    len = hello_str.size();
    if (offset < len)
    {
        if (static_cast<off_t>(offset + size) > len)
        {
            size = len - offset;
        }
        auto pos = hello_str.data();
        std::advance(pos, offset);
        memcpy(buf, pos, size);
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
    auto mountpoint = cppfuse::fs::path("/mnt/hello");
    auto file_system = cppfuse::Hello(mountpoint);
    file_system.mount();
    file_system.run();
    return 0;
}
