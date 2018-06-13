#pragma once

#include <filex/Fuse.h>

/** @brief A simple example implementation of a FUSE file system */
class Hello : public filex::FileSystem
{
public:
    std::uintmax_t file_size(const filex::Path& p) const override;

    filex::FileStatus symlink_status(const filex::Path& path) const override;

    std::vector<filex::Path>
    read_directory(const filex::Path& path) const override;

    void open(const filex::Path& path, int flags) override;

    int read(const filex::Path& path,
             filex::string_view& buffer,
             uint64_t offset) const override;

private:
    std::string hello_str = "Hello, world!\n"; /** file contents */
    filex::Path hello_path = "/hello"; /** sole path in the file system */
};
