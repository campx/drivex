#include <drivex/Permissions.h>
#include <drivex/file_status.h>
#include <drivex/file_type.h>

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

file_status::file_status(file_type file_type,
                         drivex::permissions permissions) noexcept
    : file_type_(file_type), permissions_(permissions) {}

file_status::file_status(unsigned int mode) {
  auto file_type = static_cast<ModeType>(mode & 0170000u);
  switch (file_type) {
    case ModeType::socket:
      file_type_ = file_type::socket;
      break;
    case ModeType::symlink:
      file_type_ = file_type::symlink;
      break;
    case ModeType::regular:
      file_type_ = file_type::regular;
      break;
    case ModeType::block:
      file_type_ = file_type::block;
      break;
    case ModeType::directory:
      file_type_ = file_type::directory;
      break;
    case ModeType::character:
      file_type_ = file_type::character;
      break;
    case ModeType::fifo:
      file_type_ = file_type::fifo;
      break;
    case ModeType::not_found:
      file_type_ = file_type::not_found;
      break;
  }
  permissions_ = static_cast<drivex::permissions>(mode & ~0170000u);
}

file_type file_status::type() const noexcept { return file_type_; }

permissions file_status::permissions() const noexcept { return permissions_; }

void file_status::type(file_type file_type) noexcept { file_type_ = file_type; }

void file_status::permissions(drivex::permissions permissions) noexcept {
  permissions_ = permissions;
}

file_status::operator boost::filesystem::file_status() const {
  return {static_cast<boost::filesystem::file_type>(type()),
          static_cast<boost::filesystem::perms>(permissions())};
}

file_status::operator unsigned int() const {
  auto mode = static_cast<unsigned int>(permissions());
  switch (type()) {
    case file_type::regular:
      mode |= static_cast<unsigned int>(ModeType::regular);
      break;
    case file_type::directory:
      mode |= static_cast<unsigned int>(ModeType::directory);
      break;
    case file_type::symlink:
      mode |= static_cast<unsigned int>(ModeType::symlink);
      break;
    case file_type::block:
      mode |= static_cast<unsigned int>(ModeType::block);
      break;
    case file_type::character:
      mode |= static_cast<unsigned int>(ModeType::character);
      break;
    case file_type::fifo:
      mode |= static_cast<unsigned int>(ModeType::fifo);
      break;
    case file_type::socket:
      mode |= static_cast<unsigned int>(ModeType::socket);
      break;
    case file_type::not_found:
      break;
  }
  return mode;
}

}  // namespace drivex
}  // namespace lockblox
