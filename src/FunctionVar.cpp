#include "typecheck/FunctionVar.hpp"
#include "typecheck/TypeVar.hpp"

#include "cppnotstdlib/strings.hpp"

#include <sstream>

typecheck::FunctionVar::FunctionVar() : _id(0) {}

auto typecheck::FunctionVar::name() const -> std::string {
    return this->_name;
}

auto typecheck::FunctionVar::id() const -> long long {
	return this->_id;
}

void typecheck::FunctionVar::set_id(const long long id) {
	this->_id = id;
}

auto typecheck::FunctionVar::mutable_returnvar() -> TypeVar* {
	return &this->_returnVar;
}

auto typecheck::FunctionVar::returnvar() const -> const TypeVar& {
	return this->_returnVar;
}

auto typecheck::FunctionVar::args() const -> const std::vector<TypeVar>& {
	return this->_args;
}

auto typecheck::FunctionVar::add_args() -> TypeVar* {
	this->_args.emplace_back();
	return &this->_args.at(this->_args.size() - 1);
}

auto typecheck::FunctionVar::serialize() const -> std::string {
    std::stringstream ss;

    if (this->_args.empty()) {
        ss << "<empty>";
    } else {
        for (const auto& a : _args) {
            ss << a.symbol() << ",";
        }
    }
    ss << "|";
    ss << (this->_returnVar.symbol().empty() ? "<empty>" : this->_returnVar.symbol());
    ss << "|";
    ss << (this->_name.empty() ? "<empty>" : this->_name);
    ss << "|";
    ss << this->_id;

    return ss.str();
}

auto typecheck::FunctionVar::unserialize(const std::string& str) -> FunctionVar {
    const auto parts = cppnotstdlib::string::explode(str, '|');

    const auto args = parts.at(0) == "<empty>" ? std::vector<std::string>{} : cppnotstdlib::string::explode(parts.at(0), ',');
    const auto& returnVar = parts.at(1);
    const auto& name = parts.at(2);
    const auto id = std::stol(parts.at(3));

    FunctionVar f;
    for (const auto& a : args) {
        f._args.emplace_back(a);
    }
    f._returnVar = (returnVar == "<empty>" ? "" : returnVar);
    f._name = (name == "<empty>" ? "" : name);
    f._id = id;

    return f;
}
