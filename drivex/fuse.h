#pragma once

#define FUSE_USE_VERSION 26

#include <drivex/filesystem.h>
#include <fuse/fuse.h>

namespace lockblox {
namespace drivex {

using fuse_handle = fuse;

class Fuse {
 public:
  Fuse(std::shared_ptr<filesystem> impl, Path mountpoint);
  virtual ~Fuse();

  bool is_mounted() const;
  void mount();
  void unmount();
  void run();

 private:
  std::shared_ptr<filesystem> pImpl;
  bool is_mounted_;
  const Path mountpoint_;
  fuse_chan* channel_;
  fuse_handle* fuse_;
};
}  // namespace drivex
}  // namespace lockblox
