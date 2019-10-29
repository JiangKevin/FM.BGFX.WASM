#pragma once

#include <string>
#include <sstream>

namespace cstd {
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os;
	os << value;
	return os.str();
}
}