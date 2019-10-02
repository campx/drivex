#include <drivex/error.h>

namespace lockblox {
namespace drivex {

error::error(error_code code, const std::string& description)
    : boost::filesystem::filesystem_error(
          description, boost::system::errc::make_error_code(
                           static_cast<boost::system::errc::errc_t>(code))) {}
}  // namespace drivex
}  // namespace lockblox