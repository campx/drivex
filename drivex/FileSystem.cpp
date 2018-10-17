#include <drivex/FileSystem.h>
#include <system_error>

namespace errc = boost::system::errc;
namespace filesystem = boost::filesystem;
using boost::system::error_code;
using errc::make_error_code;
using filesystem::filesystem_error;

namespace lockblox {

namespace drivex {

FileSystem::FileSystem(Path initial_path)
    : current_path_(std::move(initial_path)) {}

Path FileSystem::absolute(const Path& path) const noexcept {
  auto output = Path();
  if (path.is_relative()) {
    output /= current_path();
  }
  for (auto dir : path) {
    if (dir == "..") {
      output = parent_path(output);
    } else if (dir != ".") {
      output /= dir;
    }
  }
  return output;
}

Path FileSystem::canonical(const Path& p) const {  // POSIX realpath
  auto output = Path();
  if (p.is_relative()) {
    output = current_path();
  }

  for (auto dir : p) {
    if (dir == "/") {
      output = dir;
    } else if (dir == ".") {
      continue;
    } else if (dir == "..") {
      output = parent_path(output);
    } else {
      output /= dir;
      if (is_symlink(output)) {
        output = read_symlink(output);
        output = canonical(output);
      }
    }
  }
  return output;
}

Path FileSystem::parent_path(const Path& path) {
  return path == "/" ? path : path.parent_path();
}

const Path& FileSystem::current_path() const noexcept { return current_path_; }

void FileSystem::current_path(const Path& p) {  // POSIX chdir
  auto next_path = canonical(p);
  current_path_ = next_path;
}

// TODO default implementation
void FileSystem::copy(const Path& from, const Path& to, CopyOptions options) {
  (void)from;
  (void)to;
  (void)options;
  unsupported();
}

/* TODO default implementation, read symlink create another symlink with same
 */
void FileSystem::copy_symlink(const Path& from, const Path& to,
                              CopyOptions options) {
  (void)from;
  (void)to;
  (void)options;
  unsupported();
}

bool FileSystem::is_empty(const drivex::Path& p) const {
  (void)p;
  unsupported();
  return true;
}

void FileSystem::unsupported() const {
  throw Error(drivex::ErrorCode::function_not_supported);
}

std::uintmax_t FileSystem::file_size(const Path& path) const {
  (void)path;
  unsupported();
  return 0;
}

FileStatus FileSystem::status(const Path& path) const {
  (void)path;
  unsupported();
  return FileStatus{};
}

bool FileSystem::status_known(drivex::FileStatus s) const noexcept {
  return filesystem::status_known(s);
}

FileStatus FileSystem::symlink_status(const Path& path) const {
  return status(path);
}

Path FileSystem::read_symlink(const Path& path) const {
  (void)path;
  unsupported();
  return Path();
}

void FileSystem::create_directory(const Path& path) {
  (void)path;
  unsupported();
}

void FileSystem::create_directories(const Path& p) {
  auto fullpath = Path();
  for (auto& dir : p) {
    fullpath /= dir;
    if (!exists(fullpath)) {
      create_directory(fullpath);
    }
  }
  if (!exists(p)) {
    throw Error(drivex::ErrorCode::io_error,
                "create_directories(" + p.string() + ") failed");
  }
}

bool FileSystem::equivalent(const Path& p1, const Path& p2) const {
  auto stat1 = symlink_status(p1);
  auto stat2 = symlink_status(p2);
  auto path1 = is_symlink(stat1) ? read_symlink(p1) : p1;
  auto path2 = is_symlink(stat2) ? read_symlink(p2) : p2;
  return path1 == path2 && !is_other(stat1) && !is_other(stat2);
}

bool FileSystem::remove(const Path& path) {
  (void)path;
  unsupported();
  return true;
}

void FileSystem::create_symlink(const Path& target, const Path& link) {
  (void)target;
  (void)link;
  unsupported();
}

void FileSystem::rename(const Path& from, const Path& to) {
  (void)from;
  (void)to;
  unsupported();
}

void FileSystem::link(const Path& from, const Path& to) {
  (void)from;
  (void)to;
  unsupported();
}

void FileSystem::permissions(const Path& path, Permissions permissions) {
  (void)path;
  (void)permissions;
  unsupported();
}

void FileSystem::chown(const Path& path, uint32_t user_id, uint32_t group_id) {
  (void)path;
  (void)user_id;
  (void)group_id;
  unsupported();
}

void FileSystem::truncate(const Path& path, uint64_t offset) {
  (void)path;
  (void)offset;
  unsupported();
}

void FileSystem::open(const Path& path, int flags) {
  (void)path;
  (void)flags;
  unsupported();
}

int FileSystem::read(const Path& path, string_view& buffer,
                     uint64_t offset) const {
  (void)path;
  (void)buffer;
  (void)offset;
  unsupported();
  return 0;
}

int FileSystem::write(const Path& path, const string_view& buffer,
                      uint64_t offset) {
  (void)path;
  (void)buffer;
  (void)offset;
  unsupported();
  return 0;
}

bool FileSystem::exists(const drivex::Path& p) const {
  return exists(status(p));
}

bool FileSystem::exists(drivex::FileStatus s) const {
  return filesystem::exists(s);
}

void FileSystem::flush(const Path& path) {
  (void)path;
  unsupported();
}

void FileSystem::release(const Path& path, int flags) {
  (void)path;
  (void)flags;
  unsupported();
}

void FileSystem::fsync(const Path& path, int fd) {
  (void)path;
  (void)fd;
  unsupported();
}

void FileSystem::setxattr(const Path& path,
                          std::pair<std::string, string_view> attribute,
                          int flags) {
  (void)path;
  (void)attribute;
  (void)flags;
  unsupported();
}

std::pair<std::string, string_view> FileSystem::getxattr(
    const Path& path, const std::string& name) {
  (void)path;
  (void)name;
  unsupported();
  return std::pair<std::string, string_view>{};
}

std::vector<std::string> FileSystem::listxattr(const Path& path) {
  (void)path;
  unsupported();
  return std::vector<std::string>{};
}

void FileSystem::removexattr(const Path& path, const std::string& name) {
  (void)path;
  (void)name;
  unsupported();
}

std::vector<Path> FileSystem::read_directory(const Path& path) const {
  (void)path;
  unsupported();
  return std::vector<Path>{};
}

void FileSystem::fsyncdir(const Path& path, int datasync) {
  (void)path;
  (void)datasync;
  unsupported();
}

void FileSystem::access(const Path& path, const Permissions& permissions) {
  (void)path;
  (void)permissions;
  unsupported();
}

void FileSystem::create_file(const Path& path) {
  (void)path;
  unsupported();
}

void FileSystem::lock(const Path& path, int command) {
  (void)path;
  (void)command;
  unsupported();
}

std::time_t FileSystem::last_read_time(const Path& path) {
  (void)path;
  unsupported();
  return std::time_t{};
}

void FileSystem::last_read_time(const Path& path, std::time_t new_time) {
  (void)path;
  (void)new_time;
  unsupported();
}

std::time_t FileSystem::last_write_time(const Path& path) {
  (void)path;
  unsupported();
  return std::time_t{};
}

void FileSystem::last_write_time(const Path& path, std::time_t new_time) {
  (void)path;
  (void)new_time;
  unsupported();
}

uint64_t FileSystem::bmap(const Path& path, size_t blocksize) {
  (void)path;
  (void)blocksize;
  unsupported();
  return 0;
}

void FileSystem::ioctl(const Path& path, int cmd, void* arg, unsigned int flags,
                       void* data) {
  (void)path;
  (void)cmd;
  (void)arg;
  (void)flags;
  (void)data;
  unsupported();
}

void FileSystem::fallocate(const Path& path, int mode, uint64_t offset,
                           uint64_t length) {
  (void)path;
  (void)mode;
  (void)offset;
  (void)length;
  unsupported();
}

bool FileSystem::is_block_file(drivex::FileStatus s) const noexcept {
  return s.type() == drivex::FileType::block;
}

bool FileSystem::is_block_file(const drivex::Path& p) const {
  return is_block_file(status(p));
}

bool FileSystem::is_character_file(drivex::FileStatus s) const noexcept {
  return s.type() == drivex::FileType::character;
}

bool FileSystem::is_character_file(const drivex::Path& p) const {
  return is_character_file(status(p));
}

bool FileSystem::is_directory(drivex::FileStatus s) const noexcept {
  return s.type() == drivex::FileType::directory;
}

bool FileSystem::is_directory(const drivex::Path& p) const {
  return is_directory(status(p));
}

bool FileSystem::is_fifo(drivex::FileStatus s) const noexcept {
  return s.type() == FileType::fifo;
}

bool FileSystem::is_fifo(const drivex::Path& p) const {
  return is_fifo(status(p));
}

bool FileSystem::is_other(drivex::FileStatus s) const noexcept {
  return filesystem::is_other(s);
}

bool FileSystem::is_other(const drivex::Path& p) const {
  return is_other(status(p));
}

bool FileSystem::is_regular_file(drivex::FileStatus s) const noexcept {
  return s.type() == drivex::FileType::regular;
}

bool FileSystem::is_regular_file(const drivex::Path& p) const {
  return is_regular_file(status(p));
}

bool FileSystem::is_socket(drivex::FileStatus s) const noexcept {
  return s.type() == drivex::FileType::socket;
}

bool FileSystem::is_socket(const drivex::Path& p) const {
  return is_socket(status(p));
}

bool FileSystem::is_symlink(drivex::FileStatus s) const noexcept {
  return s.type() == drivex::FileType::symlink;
}

bool FileSystem::is_symlink(const drivex::Path& p) const {
  return is_symlink(symlink_status(p));
}
}  // namespace drivex
}  // namespace lockblox
