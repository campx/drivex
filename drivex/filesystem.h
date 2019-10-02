#pragma once

#include <drivex/Error.h>
#include <drivex/Permissions.h>
#include <drivex/file_status.h>
#include <boost/filesystem.hpp>
#include <boost/utility/string_ref.hpp>

namespace lockblox {
namespace drivex {

using Path = boost::filesystem::path;
using string_view = boost::string_ref;
using boost::filesystem::is_directory;
using CopyOptions = boost::filesystem::copy_option;

class filesystem {
 public:
  explicit filesystem(Path initial_path = Path("/"));
  virtual ~filesystem() = default;

  /** Get absolute path */
  Path absolute(const Path& path) const noexcept;

  Path canonical(const Path& p) const;

  static Path parent_path(const Path& path);

  /** Get the current path, ala POSIX getcwd */
  const Path& current_path() const noexcept;

  /** Set the current path ala POSIX chdir */
  void current_path(const Path& p);

  /** Determine whether a path exists */
  bool exists(file_status s) const;
  bool exists(const Path& p) const;

  /** Get the size of a file */
  virtual std::uintmax_t file_size(const Path& path) const;

  /** Get file attributes, following symlinks */
  virtual file_status status(const Path& path) const;

  /** Copy a file or directory */
  virtual void copy(const Path& from, const Path& to, CopyOptions options);

  /** Copy a symbolic link */
  virtual void copy_symlink(const Path& from, const Path& to,
                            CopyOptions options);

  bool status_known(file_status s) const noexcept;

  /** Get file attributes without following symlinks */
  virtual file_status symlink_status(const Path& path) const;

  /** Read the target of a symbolic link */
  virtual Path read_symlink(const Path& path) const;

  /** Create a directory */
  virtual void create_directory(const Path& path);

  /** Create all directories in the given path */
  virtual void create_directories(const Path& p);

  virtual bool equivalent(const Path& p1, const Path& p2) const;

  /** Remove a file or directory */
  virtual bool remove(const Path& path);

  /** Create a symbolic link */
  virtual void create_symlink(const Path& target, const Path& link);

  /** Rename a file */
  virtual void rename(const Path& from, const Path& to);

  /** Create a hard link to a file */
  virtual void link(const Path& from, const Path& to);

  /** Change the permission bits of a file */
  virtual void permissions(const Path& path, permissions permissions);

  virtual bool is_empty(const Path& p) const;

  // File types
  bool is_block_file(file_status s) const noexcept;
  bool is_block_file(const Path& p) const;
  bool is_character_file(file_status s) const noexcept;
  bool is_character_file(const Path& p) const;
  bool is_directory(file_status s) const noexcept;
  bool is_directory(const Path& p) const;
  bool is_fifo(file_status s) const noexcept;
  bool is_fifo(const Path& p) const;
  bool is_other(file_status s) const noexcept;
  bool is_other(const Path& p) const;
  bool is_regular_file(file_status s) const noexcept;
  bool is_regular_file(const Path& p) const;
  bool is_socket(file_status s) const noexcept;
  bool is_socket(const Path& p) const;
  bool is_symlink(file_status s) const noexcept;
  bool is_symlink(const Path& p) const;

  /** Change the owner and group of a file */
  virtual void chown(const Path& path, uint32_t user_id, uint32_t group_id);

  /** Change the size of a file */
  virtual void truncate(const Path& path, uint64_t offset);

  /** Path open operation
   *
   * Implementation should check if open is permitted for the given flags */
  virtual void open(const Path& path, int flags);

  /** Read data from an open file
   *
   * Read should return exactly the number of bytes requested except
   * on EOF or error, otherwise the rest of the data will be
   * substituted with zeroes.	*/
  virtual int read(const Path& path, string_view& buffer,
                   uint64_t offset) const;

  /** Write data to an open file
   *
   * Write should return exactly the number of bytes requested
   * except on error. */
  virtual int write(const Path& path, const string_view& buffer,
                    uint64_t offset);

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
  virtual void flush(const Path& path);

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
  virtual void release(const Path& path, int flags);

  /** Synchronize file contents
   *
   * If the datasync parameter is non-zero, then only the user data
   * should be flushed, not the meta data.
   *
   * @param fd File descriptor
   */
  virtual void fsync(const Path& path, int fd);

  /** Set extended attributes
   *
   * An attribute is a key-value pair where the key is a string and the value
   * is a binary blob */
  virtual void setxattr(const Path& path,
                        const std::pair<std::string, string_view>& attribute,
                        int flags);

  /** Get extended attributes
   *
   * An attribute is a key-value pair where the key is a string and the value
   * is a binary blob */
  virtual std::pair<std::string, string_view> getxattr(const Path& path,
                                                       const std::string& name);

  /** List extended attributes */
  virtual std::vector<std::string> listxattr(const Path& path);

  /** Remove extended attributes */
  virtual void removexattr(const Path& path, const std::string& name);

  /** Read directory */
  virtual std::vector<Path> read_directory(const Path& path) const;

  /** Synchronize directory contents
   *
   * If the datasync parameter is non-zero, then only the user data
   * should be flushed, not the meta data */
  virtual void fsyncdir(const Path& path, int datasync);

  /**
   * Check file access permissions
   *
   * This will be called for the access() system call.  If the
   * 'default_permissions' mount option is given, this method is not
   * called.
   */
  virtual void access(const Path& path, const drivex::permissions& permissions);

  /**
   * Create a file
   *
   * If the file does not exist, create it
   *
   */
  virtual void create_file(const Path& path);

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
  virtual void lock(const Path& path, int command);

  /** Get the last read time for a given path */
  virtual std::time_t last_read_time(const Path& path);

  /** Set the last read time for a given path */
  virtual void last_read_time(const Path& path, std::time_t new_time);

  /** Get the write time for a given path */
  virtual std::time_t last_write_time(const Path& path);

  /** Set the write time for a given path */
  virtual void last_write_time(const Path& path, std::time_t new_time);

  /**
   * Map block index within file to block index within device
   *
   * Note: This makes sense only for block device backed filesystems
   * mounted with the 'blkdev' option
   */
  virtual uint64_t bmap(const Path& path, size_t blocksize);

  /**
   * Ioctl
   *
   * flags will have FUSE_IOCTL_COMPAT set for 32bit ioctls in
   * 64bit environment.  The size and direction of data is
   * determined by _IOC_*() decoding of cmd.  For _IOC_NONE,
   * data will be NULL, for _IOC_WRITE data is out area, for
   * _IOC_READ in area and if both are set in/out area.  In all
   * non-NULL cases, the area is of _IOC_SIZE(cmd) bytes.
   *
   * If flags has FUSE_IOCTL_DIR then the fuse_file_info refers to a
   * directory file handle. */
  virtual void ioctl(const Path& path, int cmd, void* arg, unsigned int flags,
                     void* data);

  /**
   * Allocates space for an open file
   *
   * This function ensures that required space is allocated for specified
   * file.  If this function returns success then any subsequent write
   * request to specified range is guaranteed not to fail because of lack
   * of space on the file system media.
   */
  virtual void fallocate(const Path& path, int mode, uint64_t offset,
                         uint64_t length);

 private:
  void unsupported() const;
  Path current_path_;
};
}  // namespace drivex
}  // namespace lockblox
