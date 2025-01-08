#include "typecheck/Type.hpp"

#include "fmt/core.h"

#include <utility>

typecheck::RawType::RawType(std::string n) : _name(std::move(n)) {}

void typecheck::RawType::CopyFrom(const RawType& other) {
	this->_name = other.name();
}

auto typecheck::RawType::operator==(const RawType& other) const noexcept -> bool {
	return this->name() == other.name();
}

auto typecheck::RawType::operator!=(const RawType& other) const noexcept -> bool {
	return !(*this == other);
}

auto typecheck::RawType::name() const -> std::string {
	return this->_name;
}

void typecheck::RawType::set_name(const std::string& name) {
	this->_name = name;
}

auto typecheck::RawType::ShortDebugString() const -> std::string {
	return fmt::format(R"({ "name": "{}" })", this->_name);
}
