#pragma once
#include <drivex/error_code.h>
#include <boost/filesystem/operations.hpp>

namespace lockblox {
namespace drivex {

struct error : public boost::filesystem::filesystem_error {
  explicit error(error_code code, const std::string& description = "");
};
}  // namespace drivex
}  // namespace lockblox
