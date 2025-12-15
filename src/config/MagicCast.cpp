#include "config/MagicCast.hpp"

namespace cfg
{
// #########################################################

MagicCastException::MagicCastException(const std::string& err_msg): std::runtime_error(err_msg) {}

InvalidStringException::InvalidStringException(void): MagicCastException(INVALID_STRING_EXCEPTION) {}

InvalidVectorStringException::InvalidVectorStringException(void): MagicCastException(INVALID_VECTOR_STRING_EXCEPTION) {}

InvalidVectorUintException::InvalidVectorUintException(void): MagicCastException(INVALID_VECTOR_UINT_EXCEPTION) {}

InvalidMapStringException::InvalidMapStringException(void): MagicCastException(INVALID_MAP_STRING_EXCEPTION) {}

InvalidMapVectorStringException::InvalidMapVectorStringException(void): MagicCastException(INVALID_MAP_VECTOR_STRING_EXCEPTION) {}

AssembleTooPrimitive::AssembleTooPrimitive(void): MagicCastException(ASSEMBLE_TOO_PRIMITIVE) {}

// #########################################################
};
