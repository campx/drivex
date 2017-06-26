#include <algorithm>
#include <fusex/Fuse.h>
#include <iostream>
#include <string.h>
#include <system_error>

using string_view = std::experimental::string_view;

namespace x
{

namespace fuse
{

filesystem::FileSystem* get_impl_from_context()
{
    struct fuse_context* context = fuse_get_context();
    auto impl = static_cast<filesystem::FileSystem*>(context->private_data);
    return impl;
}

static int fusex_getattr(const char* path, struct stat* stbuf)
{
    int result = 0;
    memset(stbuf, 0, sizeof(struct stat));
    auto impl = get_impl_from_context();
    try
    {
        auto p = filesystem::Path(path);
        auto status = impl->symlink_status(p);
        auto mode = static_cast<mode_t>(status.permissions());
        switch (status.type())
        {
            case filesystem::FileType::regular:
                mode |= S_IFREG;
                break;
            case filesystem::FileType::directory:
                mode |= S_IFDIR;
                break;
            case filesystem::FileType::symlink:
                mode |= S_IFLNK;
                break;
            case filesystem::FileType::block:
                mode |= S_IFBLK;
                break;
            case filesystem::FileType::character:
                mode |= S_IFCHR;
                break;
            case filesystem::FileType::fifo:
                mode |= S_IFIFO;
                break;
            case filesystem::FileType::socket:
                mode |= S_IFSOCK;
                break;
            default:
            {
                auto ec =
                    std::make_error_code(std::errc::no_such_file_or_directory);
                throw filesystem::Error("No such file or directory", ec);
                break;
            }
        }
        stbuf->st_mode = mode;
        stbuf->st_size = impl->file_size(p);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_readlink(const char* path, char* output, size_t output_size)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto target = impl->read_symlink(filesystem::Path(path)).string();
        strncpy(output, target.c_str(), output_size);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_mkdir(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto p = filesystem::Path(path);
        impl->create_directory(p);
        impl->permissions(p, filesystem::Permissions(mode));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_unlink(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->unlink(filesystem::Path(path));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_rmdir(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->rmdir(filesystem::Path(path));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_symlink(const char* target, const char* link_path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->symlink(filesystem::Path(target), filesystem::Path(link_path));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_rename(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->rename(filesystem::Path(oldpath), filesystem::Path(newpath));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_link(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->link(filesystem::Path(oldpath), filesystem::Path(newpath));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_chmod(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->permissions(filesystem::Path(path),
                          filesystem::Permissions(mode));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_chown(const char* path, uid_t user_id, gid_t group_id)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->chown(filesystem::Path(path), user_id, group_id);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_truncate(const char* path, off_t length)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->truncate(filesystem::Path(path), length);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_open(const char* path, struct ::fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->open(filesystem::Path(path), fi->flags);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_read(const char* path,
                      char* buf,
                      size_t size,
                      off_t offset,
                      struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto buffer = string_view(buf, size);
    int result = 0;
    try
    {
        result = impl->read(filesystem::Path(path), buffer, offset);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_write(const char* path,
                       const char* buf,
                       size_t size,
                       off_t offset,
                       struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto buffer = string_view(buf, size);
    int result = 0;
    try
    {
        result = impl->write(filesystem::Path(path), buffer, offset);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_flush(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->flush(filesystem::Path(path));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_release(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->release(filesystem::Path(path), fi->flags);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_fsync(const char* path, int fd, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->fsync(filesystem::Path(path), fd);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_setxattr(const char* path,
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
        impl->setxattr(filesystem::Path(path), attribute, flags);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
fusex_getxattr(const char* path, const char* name, char* value, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto attribute =
            impl->getxattr(filesystem::Path(path), std::string(name));
        memcpy(value, attribute.second.data(), size);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_listxattr(const char* path, char* list, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    { // get attribute list then compute concatenated length
        auto attributes = impl->listxattr(filesystem::Path(path));
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
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_removexattr(const char* path, const char* name)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->removexattr(filesystem::Path(path), std::string(name));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int fusex_opendir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto status = symlink_status(filesystem::Path(path));
        if (!filesystem::is_directory(status))
        {
            auto ec = std::make_error_code(std::errc::not_a_directory);
            throw filesystem::Error("Not a directory", ec);
        }
        impl->open(filesystem::Path(path), fi->flags);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_readdir(const char* path,
                         void* buf,
                         fuse_fill_dir_t filler,
                         off_t offset,
                         struct fuse_file_info* fi)
{
    (void)offset;
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto entries = impl->read_directory(filesystem::Path(path));
        for (const auto& entry : entries)
        {
            filler(buf, entry.string().c_str(), nullptr, 0);
        }
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_releasedir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto status = symlink_status(filesystem::Path(path));
        if (!filesystem::is_directory(status))
        {
            auto ec = std::make_error_code(std::errc::not_a_directory);
            throw filesystem::Error("Not a directory", ec);
        }
        impl->release(filesystem::Path(path), fi->flags);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
fusex_fsyncdir(const char* path, int datasync, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->fsyncdir(filesystem::Path(path), datasync);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int fusex_access(const char* path, int mode)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->access(filesystem::Path(path),
                     static_cast<filesystem::Permissions>(mode));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
fusex_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = filesystem::Path(path);
        impl->create_file(p);
        impl->permissions(p, static_cast<filesystem::Permissions>(mode));
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
fusex_ftruncate(const char* path, off_t offset, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->truncate(filesystem::Path(path), offset);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
fusex_fgetattr(const char* path, struct stat* attr, struct fuse_file_info* fi)
{
    (void)fi;
    return fusex_getattr(path, attr);
}

static int fusex_lock(const char* path,
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
        impl->lock(filesystem::Path(path), cmd);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int fusex_utimens(const char* path, const struct timespec tv[2])
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = filesystem::Path(path);
        impl->last_read_time(p, tv[0].tv_sec);
        impl->last_write_time(p, tv[1].tv_sec);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int fusex_bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        *idx = impl->bmap(filesystem::Path(path), blocksize);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int fusex_ioctl(const char* path,
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
        impl->ioctl(filesystem::Path(path), cmd, arg, flags, data);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int fusex_flock(const char* path, struct fuse_file_info* fi, int op)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->lock(filesystem::Path(path), op);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int fusex_fallocate(const char* path,
                    int mode,
                    off_t offset,
                    off_t len,
                    struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->fallocate(filesystem::Path(path), mode, offset, len);
    }
    catch (const filesystem::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

Fuse::Fuse(std::shared_ptr<filesystem::FileSystem> impl,
           filesystem::Path mountpoint)
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
        operations.getattr = fusex_getattr;
        operations.readlink = fusex_readlink;
        operations.mkdir = fusex_mkdir;
        operations.unlink = fusex_unlink;
        operations.rmdir = fusex_rmdir;
        operations.symlink = fusex_symlink;
        operations.rename = fusex_rename;
        operations.link = fusex_link;
        operations.chmod = fusex_chmod;
        operations.chown = fusex_chown;
        operations.truncate = fusex_truncate;
        operations.open = fusex_open;
        operations.read = fusex_read;
        operations.write = fusex_write;
        operations.flush = fusex_flush;
        operations.release = fusex_release;
        operations.fsync = fusex_fsync;
        operations.setxattr = fusex_setxattr;
        operations.getxattr = fusex_getxattr;
        operations.listxattr = fusex_listxattr;
        operations.removexattr = fusex_removexattr;
        operations.readdir = fusex_readdir;
        operations.releasedir = fusex_releasedir;
        operations.fsyncdir = fusex_fsyncdir;
        operations.access = fusex_access;
        operations.create = fusex_create;
        operations.ftruncate = fusex_ftruncate;
        operations.lock = fusex_lock;
        operations.fgetattr = fusex_fgetattr;
        operations.utimens = fusex_utimens;
        operations.bmap = fusex_bmap;
        operations.ioctl = fusex_ioctl;
        operations.flock = fusex_flock;
        operations.fallocate = fusex_fallocate;

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

} // namespace fuse

} // namespace x
