#pragma once
#define FUSE_USE_VERSION 26
#include "FileSystem.h"
#include <fuse.h>

namespace filex
{

using fuse_handle = fuse;

class Fuse
{
public:
    Fuse(std::shared_ptr<FileSystem> impl, Path mountpoint);
    virtual ~Fuse();

    bool is_mounted() const;
    void mount();
    void unmount();
    void run();

private:
    std::shared_ptr<FileSystem> pImpl;
    bool is_mounted_;
    const Path mountpoint_;
    fuse_chan* channel_;
    fuse_handle* fuse_;
};

} // namespace filex
