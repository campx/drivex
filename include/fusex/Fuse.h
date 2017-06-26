#pragma once
#define FUSE_USE_VERSION 26
#include "FileSystem.h"
#include <fuse.h>

namespace x
{

using fuse_handle = fuse;

namespace fuse
{

class Fuse
{
public:
    Fuse(std::shared_ptr<filesystem::FileSystem> impl,
         filesystem::Path mountpoint);
    virtual ~Fuse();

    bool is_mounted() const;
    void mount();
    void unmount();
    void run();

private:
    std::shared_ptr<filesystem::FileSystem> pImpl;
    bool is_mounted_;
    const filesystem::Path mountpoint_;
    fuse_chan* channel_;
    fuse_handle* fuse_;
};

} // namespace fuse

} // namespace x
