#pragma once
#include <boost/filesystem/operations.hpp>

namespace lockblox {
namespace drivex {
enum class FileType {
  regular = boost::filesystem::regular_file,
  directory = boost::filesystem::directory_file,
  symlink = boost::filesystem::symlink_file,
  block = boost::filesystem::block_file,
  character = boost::filesystem::character_file,
  fifo = boost::filesystem::fifo_file,
  socket = boost::filesystem::socket_file,
  not_found = boost::filesystem::file_not_found
};
}
}
