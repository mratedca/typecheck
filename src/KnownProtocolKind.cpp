#include "typecheck/KnownProtocolKind.hpp"

#include "fmt/core.h"
#include "magic_enum/magic_enum.hpp"

#include <sstream>

auto typecheck::KnownProtocolKind::literal() const -> const LiteralProtocol& {
	return std::get<LiteralProtocol>(this->_data);
}

void typecheck::KnownProtocolKind::set_literal(const LiteralProtocol& literal) {
	this->_data = literal;
}

auto typecheck::KnownProtocolKind::has_literal() const -> bool {
	return std::holds_alternative<LiteralProtocol>(this->_data);
}

auto typecheck::KnownProtocolKind::ShortDebugString() const -> std::string {
	std::stringstream out;
	out << "{ ";
	if (this->has_literal()) {
		out << fmt::format(R"("literal": {})", magic_enum::enum_name(this->literal()));
	}
	out << "}";
	return out.str();
}
