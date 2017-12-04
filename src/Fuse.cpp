#include <algorithm>
#include <filex/Fuse.h>
#include <iostream>
#include <string.h>
#include <system_error>

using string_view = std::experimental::string_view;

namespace filex
{

filex::FileSystem* get_impl_from_context()
{
    struct fuse_context* context = fuse_get_context();
    auto impl = static_cast<filex::FileSystem*>(context->private_data);
    return impl;
}

static int filex_getattr(const char* path, struct stat* stbuf)
{
    int result = 0;
    memset(stbuf, 0, sizeof(struct stat));
    auto impl = get_impl_from_context();
    try
    {
        auto p = filex::Path(path);
        auto status = impl->symlink_status(p);
        auto mode = static_cast<mode_t>(status.permissions());
        switch (status.type())
        {
            case filex::FileType::regular:
                mode |= S_IFREG;
                break;
            case filex::FileType::directory:
                mode |= S_IFDIR;
                break;
            case filex::FileType::symlink:
                mode |= S_IFLNK;
                break;
            case filex::FileType::block:
                mode |= S_IFBLK;
                break;
            case filex::FileType::character:
                mode |= S_IFCHR;
                break;
            case filex::FileType::fifo:
                mode |= S_IFIFO;
                break;
            case filex::FileType::socket:
                mode |= S_IFSOCK;
                break;
            default:
            {
                throw filex::Error(std::errc::no_such_file_or_directory);
                break;
            }
        }
        stbuf->st_mode = mode;
        stbuf->st_size = impl->file_size(p);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_readlink(const char* path, char* output, size_t output_size)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto target = impl->read_symlink(filex::Path(path)).string();
        strncpy(output, target.c_str(), output_size);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_mkdir(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto p = filex::Path(path);
        impl->create_directory(p);
        impl->permissions(p, filex::Permissions(mode));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_unlink(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->remove(filex::Path(path));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_rmdir(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->remove(filex::Path(path));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_symlink(const char* target, const char* link_path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->create_symlink(filex::Path(target), filex::Path(link_path));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_rename(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->rename(filex::Path(oldpath), filex::Path(newpath));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_link(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->link(filex::Path(oldpath), filex::Path(newpath));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_chmod(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->permissions(filex::Path(path), filex::Permissions(mode));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_chown(const char* path, uid_t user_id, gid_t group_id)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->chown(filex::Path(path), user_id, group_id);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_truncate(const char* path, off_t length)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->truncate(filex::Path(path), length);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_open(const char* path, struct ::fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->open(filex::Path(path), fi->flags);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_read(const char* path,
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
        result = impl->read(filex::Path(path), buffer, offset);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_write(const char* path,
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
        result = impl->write(filex::Path(path), buffer, offset);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_flush(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->flush(filex::Path(path));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_release(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->release(filex::Path(path), fi->flags);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_fsync(const char* path, int fd, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->fsync(filex::Path(path), fd);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_setxattr(const char* path,
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
        impl->setxattr(filex::Path(path), attribute, flags);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
filex_getxattr(const char* path, const char* name, char* value, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto attribute = impl->getxattr(filex::Path(path), std::string(name));
        memcpy(value, attribute.second.data(), size);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_listxattr(const char* path, char* list, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    { // get attribute list then compute concatenated length
        auto attributes = impl->listxattr(filex::Path(path));
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
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_removexattr(const char* path, const char* name)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->removexattr(filex::Path(path), std::string(name));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int filex_opendir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto status = symlink_status(filex::Path(path));
        if (!filex::is_directory(status))
        {
            throw filex::Error(std::errc::not_a_directory);
        }
        impl->open(filex::Path(path), fi->flags);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_readdir(const char* path,
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
        auto entries = impl->read_directory(filex::Path(path));
        for (const auto& entry : entries)
        {
            filler(buf, entry.string().c_str(), nullptr, 0);
        }
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_releasedir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto status = symlink_status(filex::Path(path));
        if (!filex::is_directory(status))
        {
            throw filex::Error(std::errc::not_a_directory);
        }
        impl->release(filex::Path(path), fi->flags);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
filex_fsyncdir(const char* path, int datasync, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->fsyncdir(filex::Path(path), datasync);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int filex_access(const char* path, int mode)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->access(filex::Path(path), static_cast<filex::Permissions>(mode));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
filex_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = filex::Path(path);
        impl->create_file(p);
        impl->permissions(p, static_cast<filex::Permissions>(mode));
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
filex_ftruncate(const char* path, off_t offset, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->truncate(filex::Path(path), offset);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
filex_fgetattr(const char* path, struct stat* attr, struct fuse_file_info* fi)
{
    (void)fi;
    return filex_getattr(path, attr);
}

static int filex_lock(const char* path,
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
        impl->lock(filex::Path(path), cmd);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int filex_utimens(const char* path, const struct timespec tv[2])
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = filex::Path(path);
        impl->last_read_time(p, tv[0].tv_sec);
        impl->last_write_time(p, tv[1].tv_sec);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int filex_bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        *idx = impl->bmap(filex::Path(path), blocksize);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int filex_ioctl(const char* path,
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
        impl->ioctl(filex::Path(path), cmd, arg, flags, data);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int filex_flock(const char* path, struct fuse_file_info* fi, int op)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->lock(filex::Path(path), op);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int filex_fallocate(const char* path,
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
        impl->fallocate(filex::Path(path), mode, offset, len);
    }
    catch (const filex::Error& e)
    {
        result = -e.code().value();
    }
    return result;
}

Fuse::Fuse(std::shared_ptr<filex::FileSystem> impl, filex::Path mountpoint)
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
        operations.getattr = filex_getattr;
        operations.readlink = filex_readlink;
        operations.mkdir = filex_mkdir;
        operations.unlink = filex_unlink;
        operations.rmdir = filex_rmdir;
        operations.symlink = filex_symlink;
        operations.rename = filex_rename;
        operations.link = filex_link;
        operations.chmod = filex_chmod;
        operations.chown = filex_chown;
        operations.truncate = filex_truncate;
        operations.open = filex_open;
        operations.read = filex_read;
        operations.write = filex_write;
        operations.flush = filex_flush;
        operations.release = filex_release;
        operations.fsync = filex_fsync;
        operations.setxattr = filex_setxattr;
        operations.getxattr = filex_getxattr;
        operations.listxattr = filex_listxattr;
        operations.removexattr = filex_removexattr;
        operations.readdir = filex_readdir;
        operations.releasedir = filex_releasedir;
        operations.fsyncdir = filex_fsyncdir;
        operations.access = filex_access;
        operations.create = filex_create;
        operations.ftruncate = filex_ftruncate;
        operations.lock = filex_lock;
        operations.fgetattr = filex_fgetattr;
        operations.utimens = filex_utimens;
        operations.bmap = filex_bmap;
        operations.ioctl = filex_ioctl;
        operations.flock = filex_flock;
        operations.fallocate = filex_fallocate;

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

} // namespace filex
