#pragma once
#define FUSE_USE_VERSION 26
#include "FileSystem.h"
#include <fuse.h>

namespace cppfuse
{

class Fuse
{
public:
    Fuse(std::shared_ptr<FileSystem> impl, fs::path mountpoint);
    virtual ~Fuse();

    bool is_mounted() const;
    void mount();
    void unmount();
    void run();

private:
    std::shared_ptr<FileSystem> pImpl;
    bool is_mounted_;
    const fs::path mountpoint_;
    fuse_chan* channel_;
    fuse* fuse_;
};

} // namespace cppfuse
