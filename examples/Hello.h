#pragma once

#include <cpp-fuse/Fuse.h>

namespace cppfuse
{

/** @brief A simple example implementation of a FUSE file system */
class Hello : public Fuse
{
public:
    Hello(fs::path mountpoint);
    int getattr(const char* path, struct stat* stbuf) override;
    int readdir(const char* path,
                void* buf,
                fuse_fill_dir_t filler,
                off_t offset,
                struct fuse_file_info* fi) override;
    int open(const char* path, struct fuse_file_info* fi) override;
    int read(const char* path,
             char* buf,
             size_t size,
             off_t offset,
             struct fuse_file_info* fi) override;

private:
    std::string hello_str = "Hello, world!"; /** file contents */
    std::string hello_path = "/hello"; /** sole path in the file system */
};

} // namespace fuse
