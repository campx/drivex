#pragma once
#define FUSE_USE_VERSION 26
#include <errno.h>
#include <experimental/filesystem>
#include <experimental/string_view>
#include <fcntl.h>
#include <fuse.h>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string.h>

namespace cppfuse
{

namespace fs = std::experimental::filesystem;
using string_view = std::experimental::fundamentals_v1::string_view;
using Stat = struct stat;
using FileInfo = struct fuse_file_info;

class Fuse
{
public:
    explicit Fuse(fs::path mountpoint);
    virtual ~Fuse();

    bool is_mounted() const;
    void mount();
    void unmount();
    void run();

    virtual Stat getattr(const fs::path& path);

    virtual std::vector<fs::path>
    readdir(const fs::path& path, FileInfo& info);

    virtual void open(const fs::path& path, FileInfo& info);

    virtual int read(const fs::path& path,
                     uint64_t offset,
                     string_view& buffer,
                     FileInfo& info);

private:
    bool is_mounted_;
    const fs::path mountpoint_;
    fuse_chan* channel_;
    fuse* fuse_;
};

} // namespace cppfuse
