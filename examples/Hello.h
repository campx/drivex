#pragma once

#include <fusex/Fuse.h>

using string_view = std::experimental::string_view;

namespace x
{

/** @brief A simple example implementation of a FUSE file system */
class Hello : public filesystem::FileSystem
{
public:
    std::uintmax_t file_size(const filesystem::Path& p) override;

    filesystem::FileStatus
    symlink_status(const filesystem::Path& path) override;

    std::vector<filesystem::Path>
    read_directory(const filesystem::Path& path) override;

    void open(const filesystem::Path& path, int flags) override;

    int read(const filesystem::Path& path,
             string_view& buffer,
             uint64_t offset) override;

private:
    std::string hello_str = "Hello, world!\n"; /** file contents */
    filesystem::Path hello_path = "/hello"; /** sole path in the file system */
};

} // namespace x
