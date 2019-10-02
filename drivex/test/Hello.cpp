#include "Hello.h"
#include <iostream>

#if WIN32
auto mount_point = lockblox::drivex::Path("H:\\");
#else
auto mount_point = lockblox::drivex::Path("/mnt/hello");
#endif

std::uintmax_t Hello::file_size(const lockblox::drivex::Path& p) const {
  std::uintmax_t size = 0;
  if (p == hello_path) {
    size = hello_str.size();
  }
  return size;
}

lockblox::drivex::FileStatus Hello::symlink_status(
    const lockblox::drivex::Path& path) const {
  auto result = lockblox::drivex::FileStatus{};
  if (path == "/") {
    result.type(lockblox::drivex::FileType::directory);
    auto perms = lockblox::drivex::Permissions::owner_exec |
                 lockblox::drivex::Permissions::owner_read |
                 lockblox::drivex::Permissions::group_exec |
                 lockblox::drivex::Permissions::group_read |
                 lockblox::drivex::Permissions::others_exec |
                 lockblox::drivex::Permissions::others_read;
    result.permissions(perms);
  } else if (path == hello_path) {
    result.type(lockblox::drivex::FileType::regular);
    auto perms = lockblox::drivex::Permissions::owner_read |
                 lockblox::drivex::Permissions::group_read |
                 lockblox::drivex::Permissions::others_read;
    result.permissions(perms);
  } else {
    throw lockblox::drivex::Error(
        lockblox::drivex::ErrorCode::no_such_file_or_directory);
  }
  return result;
}

std::vector<lockblox::drivex::Path> Hello::read_directory(
    const lockblox::drivex::Path& path) const {
  (void)path;
  std::vector<lockblox::drivex::Path> result{};
  result.emplace_back(".");
  result.emplace_back("..");
  result.push_back(hello_path.filename());
  return result;
}

void Hello::open(const lockblox::drivex::Path& path, int flags) {
  if (path != hello_path) {
    throw lockblox::drivex::Error(
        lockblox::drivex::ErrorCode::no_such_file_or_directory);
  } else if ((flags & 3) != O_RDONLY) {
    throw lockblox::drivex::Error(
        lockblox::drivex::ErrorCode::permission_denied);
  }
}

int Hello::read(const lockblox::drivex::Path& path,
                lockblox::drivex::string_view& buffer, uint64_t offset) const {
  std::size_t size = buffer.size();
  if (path != hello_path) {
    throw lockblox::drivex::Error(
        lockblox::drivex::ErrorCode::no_such_file_or_directory);
  }
  std::size_t len = hello_str.size();
  if (offset < len) {
    if (offset + buffer.size() > len) {
      size = len - offset;
    }
    auto pos = hello_str.data();
    std::advance(pos, offset);
    auto output = (void*)(buffer.data());
    memcpy(output, pos, size);
  } else {
    size = 0;
  }
  return size;
}

int main(int argc, char* argv[]) {
  (void)argc;
  (void)argv;
  auto hello_fs = std::make_shared<Hello>();
  auto file_system = lockblox::drivex::Fuse(hello_fs, mount_point);
  file_system.mount();
  file_system.run();
  return 0;
}
