#include <drivex/filesystem.h>
#include <system_error>

namespace errc = boost::system::errc;
using boost::system::error_code;
using errc::make_error_code;
using errc::make_error_code;

namespace lockblox {

namespace drivex {

filesystem::filesystem(Path initial_path)
    : current_path_(std::move(initial_path)) {}

Path filesystem::absolute(const Path& path) const noexcept {
  auto output = Path();
  if (path.is_relative()) {
    output /= current_path();
  }
  for (const auto& dir : path) {
    if (dir == "..") {
      output = parent_path(output);
    } else if (dir != ".") {
      output /= dir;
    }
  }
  return output;
}

Path filesystem::canonical(const Path& p) const {  // POSIX realpath
  auto output = Path();
  if (p.is_relative()) {
    output = current_path();
  }

  for (const auto& dir : p) {
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

Path filesystem::parent_path(const Path& path) {
  return path == "/" ? path : path.parent_path();
}

const Path& filesystem::current_path() const noexcept { return current_path_; }

void filesystem::current_path(const Path& p) {  // POSIX chdir
  auto next_path = canonical(p);
  current_path_ = next_path;
}

// TODO default implementation
void filesystem::copy(const Path& from, const Path& to, CopyOptions options) {
  (void)from;
  (void)to;
  (void)options;
  unsupported();
}

/* TODO default implementation, read symlink create another symlink with same
 */
void filesystem::copy_symlink(const Path& from, const Path& to,
                              CopyOptions options) {
  (void)from;
  (void)to;
  (void)options;
  unsupported();
}

bool filesystem::is_empty(const drivex::Path& p) const {
  (void)p;
  unsupported();
  return true;
}

void filesystem::unsupported() const {
  throw error(drivex::error_code::function_not_supported);
}

std::uintmax_t filesystem::file_size(const Path& path) const {
  (void)path;
  unsupported();
  return 0;
}

file_status filesystem::status(const Path& path) const {
  (void)path;
  unsupported();
  return file_status{};
}

bool filesystem::status_known(drivex::file_status s) const noexcept {
  return boost::filesystem::status_known(s);
}

file_status filesystem::symlink_status(const Path& path) const {
  return status(path);
}

Path filesystem::read_symlink(const Path& path) const {
  (void)path;
  unsupported();
  return Path();
}

void filesystem::create_directory(const Path& path) {
  (void)path;
  unsupported();
}

void filesystem::create_directories(const Path& p) {
  auto fullpath = Path();
  for (auto& dir : p) {
    fullpath /= dir;
    if (!exists(fullpath)) {
      create_directory(fullpath);
    }
  }
  if (!exists(p)) {
    throw error(drivex::error_code::io_error,
                "create_directories(" + p.string() + ") failed");
  }
}

bool filesystem::equivalent(const Path& p1, const Path& p2) const {
  auto stat1 = symlink_status(p1);
  auto stat2 = symlink_status(p2);
  auto path1 = is_symlink(stat1) ? read_symlink(p1) : p1;
  auto path2 = is_symlink(stat2) ? read_symlink(p2) : p2;
  return path1 == path2 && !is_other(stat1) && !is_other(stat2);
}

bool filesystem::remove(const Path& path) {
  (void)path;
  unsupported();
  return true;
}

void filesystem::create_symlink(const Path& target, const Path& link) {
  (void)target;
  (void)link;
  unsupported();
}

void filesystem::rename(const Path& from, const Path& to) {
  (void)from;
  (void)to;
  unsupported();
}

void filesystem::link(const Path& from, const Path& to) {
  (void)from;
  (void)to;
  unsupported();
}

void filesystem::permissions(const Path& path,
                             drivex::permissions permissions) {
  (void)path;
  (void)permissions;
  unsupported();
}

void filesystem::chown(const Path& path, uint32_t user_id, uint32_t group_id) {
  (void)path;
  (void)user_id;
  (void)group_id;
  unsupported();
}

void filesystem::truncate(const Path& path, uint64_t offset) {
  (void)path;
  (void)offset;
  unsupported();
}

void filesystem::open(const Path& path, int flags) {
  (void)path;
  (void)flags;
  unsupported();
}

int filesystem::read(const Path& path, string_view& buffer,
                     uint64_t offset) const {
  (void)path;
  (void)buffer;
  (void)offset;
  unsupported();
  return 0;
}

int filesystem::write(const Path& path, const string_view& buffer,
                      uint64_t offset) {
  (void)path;
  (void)buffer;
  (void)offset;
  unsupported();
  return 0;
}

bool filesystem::exists(const drivex::Path& p) const {
  return exists(status(p));
}

bool filesystem::exists(drivex::file_status s) const {
  return boost::filesystem::exists(s);
}

void filesystem::flush(const Path& path) {
  (void)path;
  unsupported();
}

void filesystem::release(const Path& path, int flags) {
  (void)path;
  (void)flags;
  unsupported();
}

void filesystem::fsync(const Path& path, int fd) {
  (void)path;
  (void)fd;
  unsupported();
}

void filesystem::setxattr(const Path& path,
                          const std::pair<std::string, string_view>& attribute,
                          int flags) {
  (void)path;
  (void)attribute;
  (void)flags;
  unsupported();
}

std::pair<std::string, string_view> filesystem::getxattr(
    const Path& path, const std::string& name) {
  (void)path;
  (void)name;
  unsupported();
  return std::pair<std::string, string_view>{};
}

std::vector<std::string> filesystem::listxattr(const Path& path) {
  (void)path;
  unsupported();
  return std::vector<std::string>{};
}

void filesystem::removexattr(const Path& path, const std::string& name) {
  (void)path;
  (void)name;
  unsupported();
}

std::vector<Path> filesystem::read_directory(const Path& path) const {
  (void)path;
  unsupported();
  return std::vector<Path>{};
}

void filesystem::fsyncdir(const Path& path, int datasync) {
  (void)path;
  (void)datasync;
  unsupported();
}

void filesystem::access(const Path& path,
                        const drivex::permissions& permissions) {
  (void)path;
  (void)permissions;
  unsupported();
}

void filesystem::create_file(const Path& path) {
  (void)path;
  unsupported();
}

void filesystem::lock(const Path& path, int command) {
  (void)path;
  (void)command;
  unsupported();
}

std::time_t filesystem::last_read_time(const Path& path) {
  (void)path;
  unsupported();
  return std::time_t{};
}

void filesystem::last_read_time(const Path& path, std::time_t new_time) {
  (void)path;
  (void)new_time;
  unsupported();
}

std::time_t filesystem::last_write_time(const Path& path) {
  (void)path;
  unsupported();
  return std::time_t{};
}

void filesystem::last_write_time(const Path& path, std::time_t new_time) {
  (void)path;
  (void)new_time;
  unsupported();
}

uint64_t filesystem::bmap(const Path& path, size_t blocksize) {
  (void)path;
  (void)blocksize;
  unsupported();
  return 0;
}

void filesystem::ioctl(const Path& path, int cmd, void* arg, unsigned int flags,
                       void* data) {
  (void)path;
  (void)cmd;
  (void)arg;
  (void)flags;
  (void)data;
  unsupported();
}

void filesystem::fallocate(const Path& path, int mode, uint64_t offset,
                           uint64_t length) {
  (void)path;
  (void)mode;
  (void)offset;
  (void)length;
  unsupported();
}

bool filesystem::is_block_file(drivex::file_status s) const noexcept {
  return s.type() == drivex::file_type::block;
}

bool filesystem::is_block_file(const drivex::Path& p) const {
  return is_block_file(status(p));
}

bool filesystem::is_character_file(drivex::file_status s) const noexcept {
  return s.type() == drivex::file_type::character;
}

bool filesystem::is_character_file(const drivex::Path& p) const {
  return is_character_file(status(p));
}

bool filesystem::is_directory(drivex::file_status s) const noexcept {
  return s.type() == drivex::file_type::directory;
}

bool filesystem::is_directory(const drivex::Path& p) const {
  return is_directory(status(p));
}

bool filesystem::is_fifo(drivex::file_status s) const noexcept {
  return s.type() == file_type::fifo;
}

bool filesystem::is_fifo(const drivex::Path& p) const {
  return is_fifo(status(p));
}

bool filesystem::is_other(drivex::file_status s) const noexcept {
  return boost::filesystem::is_other(s);
}

bool filesystem::is_other(const drivex::Path& p) const {
  return is_other(status(p));
}

bool filesystem::is_regular_file(drivex::file_status s) const noexcept {
  return s.type() == drivex::file_type::regular;
}

bool filesystem::is_regular_file(const drivex::Path& p) const {
  return is_regular_file(status(p));
}

bool filesystem::is_socket(drivex::file_status s) const noexcept {
  return s.type() == drivex::file_type::socket;
}

bool filesystem::is_socket(const drivex::Path& p) const {
  return is_socket(status(p));
}

bool filesystem::is_symlink(drivex::file_status s) const noexcept {
  return s.type() == drivex::file_type::symlink;
}

bool filesystem::is_symlink(const drivex::Path& p) const {
  return is_symlink(symlink_status(p));
}
}  // namespace drivex
}  // namespace lockblox
