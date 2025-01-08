#include "typecheck/TypeVar.hpp"

#include <utility>

typecheck::TypeVar::TypeVar(std::string s) : _symbol(std::move(s)) {}

auto typecheck::TypeVar::operator==(const TypeVar& other) const noexcept -> bool {
	return this->_symbol == other._symbol;
}

auto typecheck::TypeVar::operator!=(const TypeVar& other) const noexcept -> bool {
	return !(*this == other);
}

auto typecheck::TypeVar::operator<(const TypeVar& other) const noexcept -> bool {
	return this->_symbol < other._symbol;
}

void typecheck::TypeVar::CopyFrom(const TypeVar& other) {
	this->_symbol = other.symbol();
}

auto typecheck::TypeVar::symbol() const -> std::string {
	return this->_symbol;
}

void typecheck::TypeVar::set_symbol(const std::string& s) {
	this->_symbol = s;
}

auto typecheck::TypeVar::ShortDebugString() const -> std::string {
	return "{ \"symbol\": \"" + this->_symbol + "\" }";
}
