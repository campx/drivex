#pragma once
#include <boost/system/error_code.hpp>

namespace lockblox {
namespace drivex {

enum class error_code {
  directory_not_empty = boost::system::errc::directory_not_empty,
  file_exists = boost::system::errc::file_exists,
  file_too_large = boost::system::errc::file_too_large,
  filename_too_long = boost::system::errc::filename_too_long,
  function_not_supported = boost::system::errc::function_not_supported,
  io_error = boost::system::errc::io_error,
  no_such_file_or_directory = boost::system::errc::no_such_file_or_directory,
  not_a_directory = boost::system::errc::not_a_directory,
  is_a_directory = boost::system::errc::is_a_directory,
  permission_denied = boost::system::errc::permission_denied
};
}  // namespace drivex
}  // namespace lockblox
