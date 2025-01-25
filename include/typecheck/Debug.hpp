#pragma once

#include <string>		// for string

namespace typecheck {
	void _check(bool b, const std::string& msg, const std::string& file, int line);
}

#define TYPECHECK_ASSERT(b, msg) typecheck::_check(b, msg, __FILE__, __LINE__)
