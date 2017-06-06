#include <algorithm>
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

static int cppfuse_getattr(const char* path, struct stat* stbuf)
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

static int cppfuse_readlink(const char* path, char* output, size_t output_size)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        auto target = std::string(fuse->readlink(fs::path(path)));
        strncpy(output, target.c_str(), output_size);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_mknod(const char* path, mode_t mode, dev_t device_id)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->mknod(fs::path(path), fs::perms(mode), device_id);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_mkdir(const char* path, mode_t mode)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->mkdir(fs::path(path), fs::perms(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_unlink(const char* path)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->unlink(fs::path(path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_rmdir(const char* path)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->rmdir(fs::path(path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_symlink(const char* target, const char* link_path)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->symlink(fs::path(target), fs::path(link_path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_rename(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->rename(fs::path(oldpath), fs::path(newpath));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_link(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->link(fs::path(oldpath), fs::path(newpath));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_chmod(const char* path, mode_t mode)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->chmod(fs::path(path), fs::perms(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_chown(const char* path, uid_t user_id, gid_t group_id)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->chown(fs::path(path), user_id, group_id);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_truncate(const char* path, off_t length)
{
    int result = 0;
    auto fuse = get_fuse_from_context();
    try
    {
        fuse->truncate(fs::path(path), length);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_open(const char* path, struct fuse_file_info* fi)
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

static int cppfuse_read(const char* path,
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
        result = fuse->read(fs::path(path), buffer, offset, *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_write(const char* path,
                         const char* buf,
                         size_t size,
                         off_t offset,
                         struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto buffer = string_view(buf, size);
    int result = 0;
    try
    {
        result = fuse->write(fs::path(path), buffer, offset, *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_statfs(const char* path, struct statvfs* stats)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        *stats = fuse->statfs(fs::path(path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_flush(const char* path, struct fuse_file_info* info)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        fuse->flush(fs::path(path), *info);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_release(const char* path, struct fuse_file_info* info)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        fuse->release(fs::path(path), *info);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_fsync(const char* path, int fd, struct fuse_file_info* info)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        fuse->fsync(fs::path(path), fd, *info);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_setxattr(const char* path,
                            const char* name,
                            const char* value,
                            size_t size,
                            int flags)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        auto attribute = std::make_pair(name, string_view(value, size));
        fuse->setxattr(fs::path(path), attribute, flags);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int
cppfuse_getxattr(const char* path, const char* name, char* value, size_t size)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        auto attribute = fuse->getxattr(fs::path(path), std::string(name));
        memcpy(value, attribute.second.data(), size);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_listxattr(const char* path, char* list, size_t size)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    { // get attribute list then compute concatenated length
        auto attributes = fuse->listxattr(fs::path(path));
        result = std::accumulate(attributes.begin(), attributes.end(), 0,
                                 [](int s, const std::string& attr) {
                                     return s + attr.size() + 1;
                                 });
        if (size != 0)
        {
            std::vector<char> attribute_array;
            attribute_array.reserve(result);
            for (const auto& attr : attributes)
            {
                attribute_array.insert(attribute_array.end(), attr.begin(),
                                       attr.end());
            }
            memcpy(list, attribute_array.data(), size);
        } // else just return length of array
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_removexattr(const char* path, const char* name)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        fuse->removexattr(fs::path(path), std::string(name));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_opendir(const char* path, struct fuse_file_info* info)
{
    auto fuse = get_fuse_from_context();
    int result = 0;
    try
    {
        fuse->opendir(fs::path(path), *info);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_readdir(const char* path,
                           void* buf,
                           fuse_fill_dir_t filler,
                           off_t offset,
                           struct fuse_file_info* fi)
{
    (void)offset;
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

static int cppfuse_releasedir(const char* path, struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->releasedir(fs::path(path), *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int
cppfuse_fsyncdir(const char* path, int datasync, struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->fsyncdir(fs::path(path), datasync, *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_access(const char* path, int mode)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->access(fs::path(path), static_cast<fs::perms>(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int
cppfuse_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->create(fs::path(path), static_cast<fs::perms>(mode), *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int
cppfuse_ftruncate(const char* path, off_t offset, struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->ftruncate(fs::path(path), offset, *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_fgetattr(const char* path,
                            struct stat* attr,
                            struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        *attr = fuse->fgetattr(fs::path(path), *fi);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

static int cppfuse_lock(const char* path,
                        struct fuse_file_info* fi,
                        int cmd,
                        struct flock* file_lock)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->lock(fs::path(path), *fi, cmd, *file_lock);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_utimens(const char* path, const struct timespec tv[2])
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->utimens(fs::path(path), tv[0], tv[1]);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        *idx = fuse->bmap(fs::path(path), blocksize);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_ioctl(const char* path,
                  int cmd,
                  void* arg,
                  struct fuse_file_info* fi,
                  unsigned int flags,
                  void* data)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->ioctl(fs::path(path), cmd, arg, *fi, flags, data);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_poll(const char* path,
                 struct fuse_file_info* fi,
                 struct fuse_pollhandle* ph,
                 unsigned* reventsp)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->poll(fs::path(path), *fi, ph, reventsp);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_flock(const char* path, struct fuse_file_info* fi, int op)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->flock(fs::path(path), *fi, op);
    }
    catch (const fs::filesystem_error& e)
    {
        result = e.code().value();
    }
    return result;
}

int cppfuse_fallocate(const char* path,
                      int mode,
                      off_t offset,
                      off_t len,
                      struct fuse_file_info* fi)
{
    auto fuse = get_fuse_from_context();
    auto result = 0;
    try
    {
        fuse->fallocate(fs::path(path), mode, offset, len, *fi);
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
        operations.getattr = cppfuse_getattr;
        operations.readlink = cppfuse_readlink;
        operations.mknod = cppfuse_mknod;
        operations.mkdir = cppfuse_mkdir;
        operations.unlink = cppfuse_unlink;
        operations.rmdir = cppfuse_rmdir;
        operations.symlink = cppfuse_symlink;
        operations.rename = cppfuse_rename;
        operations.link = cppfuse_link;
        operations.chmod = cppfuse_chmod;
        operations.chown = cppfuse_chown;
        operations.truncate = cppfuse_truncate;
        operations.open = cppfuse_open;
        operations.read = cppfuse_read;
        operations.write = cppfuse_write;
        operations.statfs = cppfuse_statfs;
        operations.flush = cppfuse_flush;
        operations.release = cppfuse_release;
        operations.fsync = cppfuse_fsync;
        operations.setxattr = cppfuse_setxattr;
        operations.getxattr = cppfuse_getxattr;
        operations.listxattr = cppfuse_listxattr;
        operations.removexattr = cppfuse_removexattr;
        operations.readdir = cppfuse_readdir;
        operations.releasedir = cppfuse_releasedir;
        operations.fsyncdir = cppfuse_fsyncdir;
        operations.access = cppfuse_access;
        operations.create = cppfuse_create;
        operations.ftruncate = cppfuse_ftruncate;
        operations.lock = cppfuse_lock;
        operations.fgetattr = cppfuse_fgetattr;
        operations.utimens = cppfuse_utimens;
        operations.bmap = cppfuse_bmap;
        operations.ioctl = cppfuse_ioctl;
        operations.flock = cppfuse_flock;
        operations.fallocate = cppfuse_fallocate;

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

void Fuse::unsupported() const
{
    auto ec = std::make_error_code(std::errc::function_not_supported);
    throw fs::filesystem_error("operation not supported by filesystem", ec);
}

Stat Fuse::getattr(const fs::path& path)
{
    (void)path;
    unsupported();
    return Stat();
}

fs::path Fuse::readlink(const fs::path& path)
{
    (void)path;
    unsupported();
    return fs::path();
}

void Fuse::mknod(const fs::path& path,
                 fs::perms permissions,
                 uint32_t device_id)
{
    (void)path;
    (void)permissions;
    (void)device_id;
    unsupported();
}

void Fuse::mkdir(const fs::path& path, fs::perms permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void Fuse::unlink(const fs::path& path)
{
    (void)path;
    unsupported();
}

void Fuse::rmdir(const fs::path& path)
{
    (void)path;
    unsupported();
}

void Fuse::symlink(const fs::path& target, const fs::path& link)
{
    (void)target;
    (void)link;
    unsupported();
}

void Fuse::rename(const fs::path& from, const fs::path& to)
{
    (void)from;
    (void)to;
    unsupported();
}

void Fuse::link(const fs::path& from, const fs::path& to)
{
    (void)from;
    (void)to;
    unsupported();
}

void Fuse::chmod(const fs::path& path, fs::perms permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void Fuse::chown(const fs::path& path, uint32_t user_id, uint32_t group_id)
{
    (void)path;
    (void)user_id;
    (void)group_id;
    unsupported();
}

void Fuse::truncate(const fs::path& path, uint64_t offset)
{
    (void)path;
    (void)offset;
    unsupported();
}

void Fuse::open(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
}

int Fuse::read(const fs::path& path,
               string_view& buffer,
               uint64_t offset,
               FileInfo& info)
{
    (void)path;
    (void)buffer;
    (void)offset;
    (void)info;
    unsupported();
    return 0;
}

int Fuse::write(const fs::path& path,
                const string_view& buffer,
                uint64_t offset,
                FileInfo& info)
{
    (void)path;
    (void)buffer;
    (void)offset;
    (void)info;
    unsupported();
    return 0;
}

StatVfs Fuse::statfs(const fs::path& path)
{
    (void)path;
    unsupported();
    return StatVfs();
}

void Fuse::flush(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
}

void Fuse::release(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
}

void Fuse::fsync(const fs::path& path, int fd, FileInfo& info)
{
    (void)path;
    (void)fd;
    (void)info;
    unsupported();
}

void Fuse::setxattr(const fs::path& path,
                    std::pair<std::string, string_view> attribute,
                    int flags)
{
    (void)path;
    (void)attribute;
    (void)flags;
    unsupported();
}

std::pair<std::string, string_view>
Fuse::getxattr(const fs::path& path, const std::string& name)
{
    (void)path;
    (void)name;
    unsupported();
    return std::pair<std::string, string_view>{};
}

std::vector<std::string> Fuse::listxattr(const fs::path& path)
{
    (void)path;
    unsupported();
    return std::vector<std::string>{};
}

void Fuse::removexattr(const fs::path& path, const std::string& name)
{
    (void)path;
    (void)name;
    unsupported();
}

void Fuse::opendir(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
}

std::vector<fs::path> Fuse::readdir(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
    return std::vector<fs::path>{};
}

void Fuse::releasedir(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
}

void Fuse::fsyncdir(const fs::path& path, int datasync, FileInfo& info)
{
    (void)path;
    (void)datasync;
    (void)info;
    unsupported();
}

void Fuse::access(const fs::path& path, const fs::perms& permissions)
{
    (void)path;
    (void)permissions;
    unsupported();
}

void Fuse::create(const fs::path& path, fs::perms perms, FileInfo& info)
{
    (void)path;
    (void)perms;
    (void)info;
    unsupported();
}

void Fuse::ftruncate(const fs::path& path, uint64_t size, FileInfo& info)
{
    (void)path;
    (void)size;
    (void)info;
    unsupported();
}

Stat Fuse::fgetattr(const fs::path& path, FileInfo& info)
{
    (void)path;
    (void)info;
    unsupported();
    return Stat();
}

void Fuse::lock(const fs::path& path,
                FileInfo& info,
                int command,
                Flock& file_lock)
{
    (void)path;
    (void)info;
    (void)command;
    (void)file_lock;
    unsupported();
}

void Fuse::utimens(const fs::path& path,
                   Timespec access_time,
                   Timespec mod_time)
{
    (void)path;
    (void)access_time;
    (void)mod_time;
    unsupported();
}

uint64_t Fuse::bmap(const fs::path& path, size_t blocksize)
{
    (void)path;
    (void)blocksize;
    unsupported();
    return 0;
}

void Fuse::ioctl(const fs::path& path,
                 int cmd,
                 void* arg,
                 FileInfo& info,
                 unsigned int flags,
                 void* data)
{
    (void)path;
    (void)cmd;
    (void)arg;
    (void)info;
    (void)flags;
    (void)data;
    unsupported();
}

void Fuse::poll(const fs::path& path,
                FileInfo& info,
                PollHandle* ph,
                unsigned* reventsp)
{
    (void)path;
    (void)info;
    (void)ph;
    (void)reventsp;
    unsupported();
}

void Fuse::flock(const fs::path& path, FileInfo& info, int op)
{
    (void)path;
    (void)info;
    (void)op;
    unsupported();
}

void Fuse::fallocate(const fs::path& path,
                     int mode,
                     uint64_t offset,
                     uint64_t length,
                     FileInfo& info)
{
    (void)path;
    (void)mode;
    (void)offset;
    (void)length;
    (void)info;
    unsupported();
}

} // namespace cppfuse
