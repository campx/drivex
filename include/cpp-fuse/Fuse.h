#pragma once
#define FUSE_USE_VERSION 26
#include <errno.h>
#include <experimental/filesystem>
#include <experimental/string_view>
#include <fcntl.h>
#include <fuse.h>
#include <iterator>
#include <map>
#include <stdio.h>
#include <string.h>

namespace cppfuse
{

namespace fs = std::experimental::filesystem;
using string_view = std::experimental::fundamentals_v1::string_view;
using Stat = struct stat;
using StatVfs = struct statvfs;
using FileInfo = struct fuse_file_info;
using Flock = struct flock;
using Timespec = struct timespec;

class Fuse
{
public:
    explicit Fuse(fs::path mountpoint);
    virtual ~Fuse();

    bool is_mounted() const;
    void mount();
    void unmount();
    void run();

    /** Get file attributes. */
    virtual Stat getattr(const fs::path& path);

    /** Read the target of a symbolic link */
    virtual fs::path readlink(const fs::path& path);

    /** Create a file node
     *
     * This is called for creation of all non-directory, non-symlink
     * nodes.  If the filesystem defines a create() method, then for
     * regular files that will be called instead.
     */
    virtual void
    mknod(const fs::path& path, fs::perms permissions, uint32_t device_id);

    /** Create a directory */
    virtual void mkdir(const fs::path& path, fs::perms permissions);

    /** Remove a file */
    virtual void unlink(const fs::path& path);

    /** Remove a directory */
    virtual void rmdir(const fs::path& path);

    /** Create a symbolic link */
    virtual void symlink(const fs::path& target, const fs::path& link);

    /** Rename a file */
    virtual void rename(const fs::path& from, const fs::path& to);

    /** Create a hard link to a file */
    virtual void link(const fs::path& from, const fs::path& to);

    /** Change the permission bits of a file */
    virtual void chmod(const fs::path& path, fs::perms permissions);

    /** Change the owner and group of a file */
    virtual void
    chown(const fs::path& path, uint32_t user_id, uint32_t group_id);

    /** Change the size of a file */
    virtual void truncate(const fs::path& path, uint64_t offset);

    /** File open operation
     *
     * Implementation should check if the operation is permitted for the given
     * flags */
    virtual void open(const fs::path& path, FileInfo& info);

    /** Read data from an open file
     *
     * Read should return exactly the number of bytes requested except
     * on EOF or error, otherwise the rest of the data will be
     * substituted with zeroes.	*/
    virtual int read(const fs::path& path,
                     string_view& buffer,
                     uint64_t offset,
                     FileInfo& info);

    /** Write data to an open file
     *
     * Write should return exactly the number of bytes requested
     * except on error. */
    virtual int write(const fs::path& path,
                      const string_view& buffer,
                      uint64_t offset,
                      FileInfo& info);

    /** Get file system statistics */
    virtual StatVfs statfs(const fs::path& path);

    /** Possibly flush cached data
     *
     * BIG NOTE: This is not equivalent to fsync().  It's not a
     * request to sync dirty data.
     *
     * Flush is called on each close() of a file descriptor.  So if a
     * filesystem wants to return write errors in close() and the file
     * has cached dirty data, this is a good place to write back data
     * and return any errors.  Since many applications ignore close()
     * errors this is not always useful.
     *
     * NOTE: The flush() method may be called more than once for each
     * open().	This happens if more than one file descriptor refers
     * to an opened file due to dup(), dup2() or fork() calls.	It is
     * not possible to determine if a flush is final, so each flush
     * should be treated equally.  Multiple write-flush sequences are
     * relatively rare, so this shouldn't be a problem.
     *
     * Filesystems shouldn't assume that flush will always be called
     * after some writes, or that if will be called at all.
     */
    virtual void flush(const fs::path& path, FileInfo& info);

    /** Release an open file
     *
     * Release is called when there are no more references to an open
     * file: all file descriptors are closed and all memory mappings
     * are unmapped.
     *
     * For every open() call there will be exactly one release() call
     * with the same flags and file descriptor.	 It is possible to
     * have a file opened more than once, in which case only the last
     * release will mean, that no more reads/writes will happen on the
     * file.  The return value of release is ignored.
     */
    virtual void release(const fs::path& path, FileInfo& info);

    /** Synchronize file contents
     *
     * If the datasync parameter is non-zero, then only the user data
     * should be flushed, not the meta data.
     *
     * @param fd File descriptor
     */
    virtual void fsync(const fs::path& path, int fd, FileInfo& info);

    /** Set extended attributes
     *
     * An attribute is a key-value pair where the key is a string and the value
     * is a binary blob */
    virtual void setxattr(const fs::path& path,
                          std::pair<std::string, string_view> attribute,
                          int flags);

    /** Get extended attributes
     *
     * An attribute is a key-value pair where the key is a string and the value
     * is a binary blob */
    virtual std::pair<std::string, string_view>
    getxattr(const fs::path& path, const std::string& name);

    /** List extended attributes */
    virtual std::vector<std::string> listxattr(const fs::path& path);

    /** Remove extended attributes */
    virtual void removexattr(const fs::path& path, const std::string& name);

    /** Open directory
     *
     * Unless the 'default_permissions' mount option is given,
     * this method should check if opendir is permitted for this
     * directory. Optionally opendir may also return an arbitrary
     * filehandle in the fuse_file_info structure, which will be
     * passed to readdir, closedir and fsyncdir.
     */
    virtual void opendir(const fs::path& path, FileInfo& info);

    /** Read directory */
    virtual std::vector<fs::path>
    readdir(const fs::path& path, FileInfo& info);

    /** Release directory */
    virtual void releasedir(const fs::path& path, FileInfo& info);

    /** Synchronize directory contents
     *
     * If the datasync parameter is non-zero, then only the user data
     * should be flushed, not the meta data */
    virtual void fsyncdir(const fs::path& path, int datasync, FileInfo& info);

    /**
     * Check file access permissions
     *
     * This will be called for the access() system call.  If the
     * 'default_permissions' mount option is given, this method is not
     * called.
     */
    virtual void access(const fs::path& path, const fs::perms& permissions);

    /**
     * Create and open a file
     *
     * If the file does not exist, first create it with the specified
     * mode, and then open it.
     */
    virtual void create(const fs::path& path, fs::perms perms, FileInfo& info);

    /**
     * Change the size of an open file
     *
     * This method is called instead of the truncate() method if the
     * truncation was invoked from an ftruncate() system call.
     */
    virtual void
    ftruncate(const fs::path& path, uint64_t size, FileInfo& info);

    /**
         * Get attributes from an open file
         *
         * This method is called instead of the getattr() method if the
         * file information is available.
         *
         * Currently this is only called after the create() method if that
         * is implemented (see above).  Later it may be called for
         * invocations of fstat() too. */
    virtual Stat fgetattr(const fs::path& path, FileInfo& info);

    /**
     * Perform POSIX file locking operation
     *
     * The cmd argument will be either F_GETLK, F_SETLK or F_SETLKW.
     *
     * For the meaning of fields in 'struct flock' see the man page
     * for fcntl(2).  The l_whence field will always be set to
     * SEEK_SET.
     *
     * For checking lock ownership, the 'fuse_file_info->owner'
     * argument must be used.
     *
     * For F_GETLK operation, the library will first check currently
     * held locks, and if a conflicting lock is found it will return
     * information without calling this method.	 This ensures, that
     * for local locks the l_pid field is correctly filled in.	The
     * results may not be accurate in case of race conditions and in
     * the presence of hard links, but it's unlikely that an
     * application would rely on accurate GETLK results in these
     * cases.  If a conflicting lock is not found, this method will be
     * called, and the filesystem may fill out l_pid by a meaningful
     * value, or it may leave this field zero.
     *
     * For F_SETLK and F_SETLKW the l_pid field will be set to the pid
     * of the process performing the locking operation.
     *
     * Note: if this method is not implemented, the kernel will still
     * allow file locking to work locally.  Hence it is only
     * interesting for network filesystems and similar.
     */
    virtual void
    lock(const fs::path& path, FileInfo& info, int command, Flock& file_lock);

    /**
     * Change the access and modification times of a file with
     * nanosecond resolution
     *
     * This supersedes the old utime() interface.  New applications
     * should use this.
     *
     * See the utimensat(2) man page for details.
     */
    virtual void
    utimens(const fs::path& path, Timespec access_time, Timespec mod_time);

    /**
     * Map block index within file to block index within device
     *
     * Note: This makes sense only for block device backed filesystems
     * mounted with the 'blkdev' option
     */
    virtual uint64_t bmap(const fs::path& path, size_t blocksize);

private:
    void unsupported() const;
    bool is_mounted_;
    const fs::path mountpoint_;
    fuse_chan* channel_;
    fuse* fuse_;
};

} // namespace cppfuse
