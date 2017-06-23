#include <algorithm>
#include <cpp-fuse/Fuse.h>
#include <iostream>
#include <string.h>
#include <system_error>

using errc = std::errc;
using std::make_error_code;

namespace cppfuse
{

FileSystem* get_impl_from_context()
{
    struct fuse_context* context = fuse_get_context();
    auto impl = static_cast<FileSystem*>(context->private_data);
    return impl;
}

static int cppfuse_getattr(const char* path, struct stat* stbuf)
{
    int result = 0;
    memset(stbuf, 0, sizeof(struct stat));
    auto impl = get_impl_from_context();
    try
    {
        auto p = fs::path(path);
        auto status = impl->symlink_status(p);
        auto mode = static_cast<mode_t>(status.permissions());
        switch (status.type())
        {
            case fs::file_type::regular:
                mode |= S_IFREG;
                break;
            case fs::file_type::directory:
                mode |= S_IFDIR;
                break;
            case fs::file_type::symlink:
                mode |= S_IFLNK;
                break;
            case fs::file_type::block:
                mode |= S_IFBLK;
                break;
            case fs::file_type::character:
                mode |= S_IFCHR;
                break;
            case fs::file_type::fifo:
                mode |= S_IFIFO;
                break;
            case fs::file_type::socket:
                mode |= S_IFSOCK;
                break;
            default:
            {
                auto ec = make_error_code(errc::no_such_file_or_directory);
                throw fs::filesystem_error("No such file or directory", ec);
                break;
            }
        }
        stbuf->st_mode = mode;
        stbuf->st_size = impl->file_size(p);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_readlink(const char* path, char* output, size_t output_size)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto target = impl->read_symlink(fs::path(path)).string();
        strncpy(output, target.c_str(), output_size);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_mkdir(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        auto p = fs::path(path);
        impl->create_directory(p);
        impl->permissions(p, fs::perms(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_unlink(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->unlink(fs::path(path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_rmdir(const char* path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->rmdir(fs::path(path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_symlink(const char* target, const char* link_path)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->symlink(fs::path(target), fs::path(link_path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_rename(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->rename(fs::path(oldpath), fs::path(newpath));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_link(const char* oldpath, const char* newpath)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->link(fs::path(oldpath), fs::path(newpath));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_chmod(const char* path, mode_t mode)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->permissions(fs::path(path), fs::perms(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_chown(const char* path, uid_t user_id, gid_t group_id)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->chown(fs::path(path), user_id, group_id);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_truncate(const char* path, off_t length)
{
    int result = 0;
    auto impl = get_impl_from_context();
    try
    {
        impl->truncate(fs::path(path), length);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_open(const char* path, struct ::fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->open(fs::path(path), fi->flags);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_read(const char* path,
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
        result = impl->read(fs::path(path), buffer, offset);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_write(const char* path,
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
        result = impl->write(fs::path(path), buffer, offset);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_flush(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->flush(fs::path(path));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_release(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->release(fs::path(path), fi->flags);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_fsync(const char* path, int fd, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->fsync(fs::path(path), fd);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_setxattr(const char* path,
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
        impl->setxattr(fs::path(path), attribute, flags);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
cppfuse_getxattr(const char* path, const char* name, char* value, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto attribute = impl->getxattr(fs::path(path), std::string(name));
        memcpy(value, attribute.second.data(), size);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_listxattr(const char* path, char* list, size_t size)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    { // get attribute list then compute concatenated length
        auto attributes = impl->listxattr(fs::path(path));
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
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_removexattr(const char* path, const char* name)
{
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        impl->removexattr(fs::path(path), std::string(name));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int cppfuse_opendir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    int result = 0;
    try
    {
        auto status = symlink_status(fs::path(path));
        if (!fs::is_directory(status))
        {
            auto ec = make_error_code(errc::not_a_directory);
            throw fs::filesystem_error("Not a directory", ec);
        }
        impl->open(fs::path(path), fi->flags);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
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
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto entries = impl->read_directory(fs::path(path));
        for (const auto& entry : entries)
        {
            filler(buf, entry.string().c_str(), nullptr, 0);
        }
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_releasedir(const char* path, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto status = symlink_status(fs::path(path));
        if (!fs::is_directory(status))
        {
            auto ec = make_error_code(errc::not_a_directory);
            throw fs::filesystem_error("Not a directory", ec);
        }
        impl->release(fs::path(path), fi->flags);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
cppfuse_fsyncdir(const char* path, int datasync, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->fsyncdir(fs::path(path), datasync);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_access(const char* path, int mode)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->access(fs::path(path), static_cast<fs::perms>(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
cppfuse_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = fs::path(path);
        impl->create_file(p);
        impl->permissions(p, static_cast<fs::perms>(mode));
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int
cppfuse_ftruncate(const char* path, off_t offset, struct fuse_file_info* fi)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->truncate(fs::path(path), offset);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

static int cppfuse_fgetattr(const char* path,
                            struct stat* attr,
                            struct fuse_file_info* fi)
{
    (void)fi;
    return cppfuse_getattr(path, attr);
}

static int cppfuse_lock(const char* path,
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
        impl->lock(fs::path(path), cmd);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int cppfuse_utimens(const char* path, const struct timespec tv[2])
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        auto p = fs::path(path);
        impl->last_read_time(p, tv[0].tv_sec);
        impl->last_write_time(p, tv[1].tv_sec);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int cppfuse_bmap(const char* path, size_t blocksize, uint64_t* idx)
{
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        *idx = impl->bmap(fs::path(path), blocksize);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
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
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->ioctl(fs::path(path), cmd, arg, flags, data);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int cppfuse_flock(const char* path, struct fuse_file_info* fi, int op)
{
    (void)fi;
    auto impl = get_impl_from_context();
    auto result = 0;
    try
    {
        impl->lock(fs::path(path), op);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

int cppfuse_fallocate(const char* path,
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
        impl->fallocate(fs::path(path), mode, offset, len);
    }
    catch (const fs::filesystem_error& e)
    {
        result = -e.code().value();
    }
    return result;
}

Fuse::Fuse(std::shared_ptr<FileSystem> impl, fs::path mountpoint)
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
        operations.getattr = cppfuse_getattr;
        operations.readlink = cppfuse_readlink;
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

} // namespace cppfuse
