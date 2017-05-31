#pragma once

#include <cpp-fuse/Fuse.h>

namespace cppfuse
{

/** @brief A simple example implementation of a FUSE file system */
class Hello : public Fuse
{
public:
    explicit Hello(fs::path mountpoint);
    Stat getattr(const fs::path& path) override;

    std::vector<fs::path>
    readdir(const fs::path& path, FileInfo& info) override;

    void open(const fs::path& path, FileInfo& info) override;

    int read(const fs::path& path,
             uint64_t offset,
             string_view& buffer,
             FileInfo& info) override;

private:
    std::string hello_str = "Hello, world!"; /** file contents */
    fs::path hello_path = "/hello"; /** sole path in the file system */
};

} // namespace cppfuse
