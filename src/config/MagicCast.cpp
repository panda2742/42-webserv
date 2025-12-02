#include "config/MagicCast.hpp"

namespace Config
{
// #########################################################

MagicCastException::MagicCastException(const std::string& err_msg): std::runtime_error(err_msg) {}
InvalidCastException::InvalidCastException(void): MagicCastException(INVALID_CAST_EXCEPTION) {}

// #########################################################
};
