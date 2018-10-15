#pragma once
#include <drivex/ErrorCode.h>
#include <boost/filesystem/operations.hpp>

namespace lockblox {
namespace drivex {

struct Error : public boost::filesystem::filesystem_error {
  explicit Error(ErrorCode code, std::string description = "");
};
}  // namespace drivex
}  // namespace lockblox
