#pragma once
#define FUSE_USE_VERSION 26
#include <errno.h>
#include <experimental/filesystem>
#include <fcntl.h>
#include <fuse.h>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string.h>

namespace cppfuse
{

namespace fs = std::experimental::filesystem;

class Fuse
{
public:
    Fuse(fs::path mountpoint);
    virtual ~Fuse();

    bool is_mounted() const;
    void mount();
    void unmount();
    void run();

    virtual int getattr(const char* path, struct stat* stbuf);
    virtual int readdir(const char* path,
                        void* buf,
                        fuse_fill_dir_t filler,
                        off_t offset,
                        struct fuse_file_info* fi);
    virtual int open(const char* path, struct fuse_file_info* fi);
    virtual int read(const char* path,
                     char* buf,
                     size_t size,
                     off_t offset,
                     struct fuse_file_info* fi);

private:
    bool is_mounted_;
    const fs::path mountpoint_;
    fuse_chan* channel_;
    fuse* fuse_;
};

} // namespace fuse
