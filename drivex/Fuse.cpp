#include <algorithm>
#include <drivex/Fuse.h>
#include <iostream>

#if WIN32
#define S_IFIFO 0x1000;
#define S_IFBLK 0x3000;
#define S_IFSOCK S_IFREG; // treat socket as regular file on Windows
#define OFF_T long long int
#else
#define OFF_T off_t
#define FUSE_STAT struct stat
#endif

namespace errc = boost::system::errc;

namespace drivex
{

drivex::FileSystem* get_impl_from_context()
{
    struct fuse_context* context = fuse_get_context();
    auto impl = static_cast<drivex::FileSystem*>(context->private_data);
    return impl;
}

static int drivex_getattr(const char* path, FUSE_STAT* stbuf)
{
    int result = 0;
    memset(stbuf, 0, sizeof(struct stat));
    auto impl = get_impl_from_context();
    try
    {
        auto p = drivex::Path(path);
        auto status = impl->symlink_status(p);
        auto mode = static_cast<mode_t>(status.permissions());
        switch (status.type())
        {
            case drivex::FileType::regular:
                mode |= S_IFREG;
                break;
            case drivex::FileType::directory:
                mode |= S_IFDIR;
                break;
            case drivex::FileType::symlink:
                mode |= S_IFLNK;
                break;
            case drivex::FileType::block:
                mode |= S_IFBLK;
                break;
            case drivex::FileType::character:
                mode |= S_IFCHR;
                break;
            case drivex::FileType::fifo:
                mode |= S_IFIFO;
                break;
            case drivex::FileType::socket:
                mode |= S_IFSOCK;
                break;
            case drivex::FileType::not_found:
                break;
        }
        stbuf->st_mode = mode;
        stbuf->st_size = impl->file_size(p);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_readlink(const char* path, char* output, size_t output_size)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto target = impl->read_symlink(drivex::Path(path)).string();
        strncpy(output, target.c_str(), output_size);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_mkdir(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto p = drivex::Path(path);
        impl->create_directory(p);
        impl->permissions(p, drivex::Permissions(mode));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_unlink(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->remove(drivex::Path(path));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_rmdir(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->remove(drivex::Path(path));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_symlink(const char* target, const char* link_path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->create_symlink(drivex::Path(target), drivex::Path(link_path));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_rename(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->rename(drivex::Path(oldpath), drivex::Path(newpath));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_link(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->link(drivex::Path(oldpath), drivex::Path(newpath));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_chmod(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->permissions(drivex::Path(path), drivex::Permissions(mode));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_chown(const char* path, uid_t user_id, gid_t group_id)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->chown(drivex::Path(path), user_id, group_id);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_truncate(const char* path, OFF_T length)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->truncate(drivex::Path(path), length);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_open(const char* path, struct ::fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->open(drivex::Path(path), fi->flags);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_read(const char* path,
                      char* buf,
                      size_t size,
                      OFF_T offset,
                      struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto buffer = string_view(buf, size);
    int result = 0;
    try
    {
        result = impl->read(drivex::Path(path), buffer, offset);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_write(const char* path,
                       const char* buf,
                       size_t size,
                       OFF_T offset,
                       struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto buffer = string_view(buf, size);
    int result = 0;
    try
    {
        result = impl->write(drivex::Path(path), buffer, offset);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_flush(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->flush(drivex::Path(path));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_release(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->release(drivex::Path(path), fi->flags);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_fsync(const char* path, int fd, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->fsync(drivex::Path(path), fd);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_setxattr(const char* path,
                          const char* name,
                          const char* value,
                          size_t size,
                          int flags)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto attribute = std::make_pair(name, string_view(value, size));
        impl->setxattr(drivex::Path(path), attribute, flags);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
drivex_getxattr(const char* path, const char* name, char* value, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto attribute = impl->getxattr(drivex::Path(path), std::string(name));
        memcpy(value, attribute.second.data(), size);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_listxattr(const char* path, char* list, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    { // get attribute list then compute concatenated length
        auto attributes = impl->listxattr(drivex::Path(path));
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
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_removexattr(const char* path, const char* name)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->removexattr(drivex::Path(path), std::string(name));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int drivex_opendir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto status = symlink_status(drivex::Path(path));
        if (!drivex::is_directory(status))
        {
            throw drivex::Error(drivex::ErrorCode::not_a_directory);
        }
        impl->open(drivex::Path(path), fi->flags);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_readdir(const char* path,
                         void* buf,
                         fuse_fill_dir_t filler,
                         OFF_T offset,
                         struct fuse_file_info* fi)
{
    (void)offset;
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto entries = impl->read_directory(drivex::Path(path));
        for (const auto& entry : entries)
        {
            filler(buf, entry.string().c_str(), nullptr, 0);
        }
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_releasedir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto status = symlink_status(drivex::Path(path));
        if (!drivex::is_directory(status))
        {
            throw drivex::Error(drivex::ErrorCode::not_a_directory);
        }
        impl->release(drivex::Path(path), fi->flags);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
drivex_fsyncdir(const char* path, int datasync, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->fsyncdir(drivex::Path(path), datasync);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int drivex_access(const char* path, int mode)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->access(drivex::Path(path), static_cast<drivex::Permissions>(mode));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
drivex_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = drivex::Path(path);
        impl->create_file(p);
        impl->permissions(p, static_cast<drivex::Permissions>(mode));
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
drivex_ftruncate(const char* path, OFF_T offset, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->truncate(drivex::Path(path), offset);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
drivex_fgetattr(const char* path, FUSE_STAT* attr, struct fuse_file_info* fi)
{
    (void)fi;
    return drivex_getattr(path, attr);
}

static int drivex_lock(const char* path,
                      struct fuse_file_info* fi,
                      int cmd,
                      struct flock* file_lock)
{
    (void)fi;
    (void)file_lock;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->lock(drivex::Path(path), cmd);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int drivex_utimens(const char* path, const struct timespec tv[2])
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = drivex::Path(path);
        impl->last_read_time(p, tv[0].tv_sec);
        impl->last_write_time(p, tv[1].tv_sec);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int drivex_bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        *idx = impl->bmap(drivex::Path(path), blocksize);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int drivex_ioctl(const char* path,
                int cmd,
                void* arg,
                struct fuse_file_info* fi,
                unsigned int flags,
                void* data)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->ioctl(drivex::Path(path), cmd, arg, flags, data);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int drivex_flock(const char* path, struct fuse_file_info* fi, int op)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->lock(drivex::Path(path), op);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int drivex_fallocate(const char* path,
                    int mode,
                    OFF_T offset,
                    OFF_T len,
                    struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->fallocate(drivex::Path(path), mode, offset, len);
    }
    catch (const drivex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

Fuse::Fuse(std::shared_ptr<drivex::FileSystem> impl, drivex::Path mountpoint)
    : pImpl(std::move(impl)), is_mounted_(false),
      mountpoint_(std::move(mountpoint)),
      channel_(fuse_mount(mountpoint_.string().c_str(), nullptr))
{
}

Fuse::~Fuse() { unmount(); }

bool Fuse::is_mounted() const { return is_mounted_; }

void Fuse::mount()
{
    if (!is_mounted() && nullptr != channel_)
    {
        auto operations = fuse_operations{};
        operations.getattr = drivex_getattr;
        operations.readlink = drivex_readlink;
        operations.mkdir = drivex_mkdir;
        operations.unlink = drivex_unlink;
        operations.rmdir = drivex_rmdir;
        operations.symlink = drivex_symlink;
        operations.rename = drivex_rename;
        operations.link = drivex_link;
        operations.chmod = drivex_chmod;
        operations.chown = drivex_chown;
        operations.truncate = drivex_truncate;
        operations.open = drivex_open;
        operations.read = drivex_read;
        operations.write = drivex_write;
        operations.flush = drivex_flush;
        operations.release = drivex_release;
        operations.fsync = drivex_fsync;
        operations.setxattr = drivex_setxattr;
        operations.getxattr = drivex_getxattr;
        operations.listxattr = drivex_listxattr;
        operations.removexattr = drivex_removexattr;
        operations.readdir = drivex_readdir;
        operations.releasedir = drivex_releasedir;
        operations.fsyncdir = drivex_fsyncdir;
        operations.access = drivex_access;
        operations.create = drivex_create;
        operations.ftruncate = drivex_ftruncate;
        operations.lock = drivex_lock;
        operations.fgetattr = drivex_fgetattr;
        operations.utimens = drivex_utimens;
        operations.bmap = drivex_bmap;
#if !WIN32
        operations.ioctl = drivex_ioctl;
        operations.flock = drivex_flock;
        operations.fallocate = drivex_fallocate;
#endif

        auto ops_size = sizeof(operations);
        auto user_data = reinterpret_cast<void*>(pImpl.get());
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
        fuse_unmount(mountpoint_.string().c_str(), channel_);
    }
}

} // namespace drivex
