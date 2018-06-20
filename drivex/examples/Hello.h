#pragma once

#include <drivex/Fuse.h>

/** @brief A simple example implementation of a FUSE file system */
class Hello : public drivex::FileSystem
{
public:
    std::uintmax_t file_size(const drivex::Path& p) const override;

    drivex::FileStatus symlink_status(const drivex::Path& path) const override;

    std::vector<drivex::Path>
    read_directory(const drivex::Path& path) const override;

    void open(const drivex::Path& path, int flags) override;

    int read(const drivex::Path& path,
             drivex::string_view& buffer,
             uint64_t offset) const override;

private:
    std::string hello_str = "Hello, world!\n"; /** file contents */
    drivex::Path hello_path = "/hello"; /** sole path in the file system */
};
