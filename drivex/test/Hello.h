#pragma once

#include <drivex/Fuse.h>

/** @brief A simple example implementation of a FUSE file system */
class Hello : public lockblox::drivex::FileSystem {
 public:
  std::uintmax_t file_size(const lockblox::drivex::Path& p) const override;

  lockblox::drivex::FileStatus symlink_status(
      const lockblox::drivex::Path& path) const override;

  std::vector<lockblox::drivex::Path> read_directory(
      const lockblox::drivex::Path& path) const override;

  void open(const lockblox::drivex::Path& path, int flags) override;

  int read(const lockblox::drivex::Path& path,
           lockblox::drivex::string_view& buffer,
           uint64_t offset) const override;

 private:
  std::string hello_str = "Hello, world!\n"; /** file contents */
  lockblox::drivex::Path hello_path =
      "/hello"; /** sole path in the file system */
};
