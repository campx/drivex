#include <drivex/FileStatus.h>
#include <drivex/FileType.h>
#include <drivex/Permissions.h>

namespace {

enum class ModeType : unsigned int {
  socket = 0140000,    /**< Socket */
  symlink = 0120000,   /**< Symlink */
  regular = 0100000,   /**< Regular file */
  block = 0060000,     /**< Block device */
  directory = 0040000, /**< Directory */
  character = 0020000, /** Character device */
  fifo = 0010000,      /** FIFO */
  not_found = 0        /** File not found */
};

}  // namespace

namespace lockblox {
namespace drivex {

FileStatus::FileStatus(FileType file_type, Permissions permissions) noexcept
    : file_type_(file_type), permissions_(permissions) {}

FileStatus::FileStatus(unsigned int mode) {
  auto file_type = static_cast<ModeType>(mode & 0170000u);
  switch (file_type) {
    case ModeType::socket:
      file_type_ = FileType::socket;
      break;
    case ModeType::symlink:
      file_type_ = FileType::symlink;
      break;
    case ModeType::regular:
      file_type_ = FileType::regular;
      break;
    case ModeType::block:
      file_type_ = FileType::block;
      break;
    case ModeType::directory:
      file_type_ = FileType::directory;
      break;
    case ModeType::character:
      file_type_ = FileType::character;
      break;
    case ModeType::fifo:
      file_type_ = FileType::fifo;
      break;
    case ModeType::not_found:
      file_type_ = FileType::not_found;
      break;
  }
  permissions_ = static_cast<Permissions>(mode & ~0170000u);
}

FileType FileStatus::type() const noexcept { return file_type_; }

Permissions FileStatus::permissions() const noexcept { return permissions_; }

void FileStatus::type(FileType file_type) noexcept { file_type_ = file_type; }

void FileStatus::permissions(Permissions permissions) noexcept {
  permissions_ = permissions;
}

FileStatus::operator boost::filesystem::file_status() const {
  return boost::filesystem::file_status(
      static_cast<boost::filesystem::file_type>(type()),
      static_cast<boost::filesystem::perms>(permissions()));
}

FileStatus::operator unsigned int() const {
  auto mode = static_cast<unsigned int>(permissions());
  switch (type()) {
    case FileType::regular:
      mode |= static_cast<unsigned int>(ModeType::regular);
      break;
    case FileType::directory:
      mode |= static_cast<unsigned int>(ModeType::directory);
      break;
    case FileType::symlink:
      mode |= static_cast<unsigned int>(ModeType::symlink);
      break;
    case FileType::block:
      mode |= static_cast<unsigned int>(ModeType::block);
      break;
    case FileType::character:
      mode |= static_cast<unsigned int>(ModeType::character);
      break;
    case FileType::fifo:
      mode |= static_cast<unsigned int>(ModeType::fifo);
      break;
    case FileType::socket:
      mode |= static_cast<unsigned int>(ModeType::socket);
      break;
    case FileType::not_found:
      break;
  }
  return mode;
}

}  // namespace drivex
}  // namespace lockblox
