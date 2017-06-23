#pragma once

#include <cpp-fuse/Fuse.h>

namespace cppfuse
{

/** @brief A simple example implementation of a FUSE file system */
class Hello : public FileSystem
{
public:
    std::uintmax_t file_size(const fs::path& p) override;

    fs::file_status symlink_status(const fs::path& path) override;

    std::vector<fs::path> read_directory(const fs::path& path) override;

    void open(const fs::path& path, int flags) override;

    int
    read(const fs::path& path, string_view& buffer, uint64_t offset) override;

private:
    std::string hello_str = "Hello, world!\n"; /** file contents */
    fs::path hello_path = "/hello"; /** sole path in the file system */
};

} // namespace cppfuse
