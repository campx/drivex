#include <cpp-fuse/Fuse.h>
#include <iostream>

namespace cppfuse
{

Fuse* get_fuse_from_context()
{
    struct fuse_context* context = fuse_get_context();
    auto fuse = static_cast<Fuse*>(context->private_data);
    return fuse;
}

static int fuse_getattr(const char* path, struct stat* stbuf)
{
    int result = 0;
    memset(stbuf, 0, sizeof(struct stat));
    auto fuse = get_fuse_from_context();
    try
    {
        *stbuf = fuse->getattr(path);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int fuse_readdir(const char* path,
                        void* buf,
                        fuse_fill_dir_t filler,
                        off_t offset,
                        struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        auto entries = fuse->readdir(fs::path(path), *fi);
        for (const auto& entry : entries)
        {
            filler(buf, std::string(entry).c_str(), nullptr, 0);
        }
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int fuse_open(const char* path, struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->open(fs::path(path), *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int fuse_read(const char* path,
                     char* buf,
                     size_t size,
                     off_t offset,
                     struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto buffer = string_view(buf, size);
    int result = 0;
    try
    {
        result = fuse->read(fs::path(path), offset, buffer, *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

Fuse::Fuse(fs::path mountpoint)
    : is_mounted_(false), mountpoint_(std::move(mountpoint)),
      channel_(fuse_mount(std::string(mountpoint_).c_str(), nullptr))
{
}

Fuse::~Fuse() { unmount(); }

bool Fuse::is_mounted() const { return is_mounted_; }

void Fuse::mount()
{
    if (!is_mounted() && nullptr != channel_)
    {
        auto operations = fuse_operations{};
        operations.getattr = fuse_getattr;
        operations.readdir = fuse_readdir;
        operations.open = fuse_open;
        operations.read = fuse_read;
        auto ops_size = sizeof(operations);
        auto user_data = reinterpret_cast<void*>(this);
        auto args = nullptr;
        fuse_ = fuse_new(channel_, args, &operations, ops_size, user_data);
        is_mounted_ = nullptr != fuse_;
    }
}

void Fuse::run()
{
    if (is_mounted())
    {
        fuse_loop(fuse_);
    }
}

void Fuse::unmount()
{
    if (is_mounted() && nullptr != channel_)
    {
        fuse_unmount(std::string(mountpoint_).c_str(), channel_);
    }
}

Stat Fuse::getattr(const fs::path& path)
{
    auto ec = std::make_error_code(std::errc::function_not_supported);
    throw fs::filesystem_error("getattr is unsupported in this filesystem",
                               ec);
    return Stat();
}

std::vector<fs::path> Fuse::readdir(const fs::path& path, FileInfo& info)
{
    auto ec = std::make_error_code(std::errc::function_not_supported);
    throw fs::filesystem_error("readdir is unsupported in this filesystem",
                               ec);
    return std::vector<fs::path>{};
}

void Fuse::open(const fs::path& path, FileInfo& info)
{
    auto ec = std::make_error_code(std::errc::function_not_supported);
    throw fs::filesystem_error("open is unsupported in this filesystem", ec);
}

int Fuse::read(const fs::path& path,
               uint64_t offset,
               string_view& buffer,
               FileInfo& info)
{
    auto ec = std::make_error_code(std::errc::function_not_supported);
    throw fs::filesystem_error("read is unsupported in this filesystem", ec);
    return 0;
}

} // namespace cppfuse
