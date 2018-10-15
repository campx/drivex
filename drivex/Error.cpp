#include <drivex/Error.h>

namespace lockblox {
namespace drivex {

Error::Error(ErrorCode code, std::string description)
    : boost::filesystem::filesystem_error(
          std::move(description),
          boost::system::errc::make_error_code(
              static_cast<boost::system::errc::errc_t>(code))) {}
}  // namespace drivex
}  // namespace lockblox