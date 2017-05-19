#include <cpp-fuse/Fuse.h>

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
    auto fuse = get_fuse_from_context();
    return fuse->getattr(path, stbuf);
}

static int fuse_readdir(const char* path,
                        void* buf,
                        fuse_fill_dir_t filler,
                        off_t offset,
                        struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    return fuse->readdir(path, buf, filler, offset, fi);
}

static int fuse_open(const char* path, struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    return fuse->open(path, fi);
}

static int fuse_read(const char* path,
                     char* buf,
                     size_t size,
                     off_t offset,
                     struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    return fuse->read(path, buf, size, offset, fi);
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

int Fuse::getattr(const char* path, struct stat* stbuf) { return -ENOSYS; }

int Fuse::readdir(const char* path,
                  void* buf,
                  fuse_fill_dir_t filler,
                  off_t offset,
                  struct fuse_file_info* fi)
{
    return -ENOSYS;
}

int Fuse::open(const char* path, struct fuse_file_info* fi) { return -ENOSYS; }

int Fuse::read(const char* path,
               char* buf,
               size_t size,
               off_t offset,
               struct fuse_file_info* fi)
{
    return -ENOSYS;
}

} // namespace cppfuse
