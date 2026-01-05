#include "typecheck/GenericType.hpp"
#include "typecheck/Type.hpp"

#include <sstream>

typecheck::GenericType::GenericType(std::string n) : _name(std::move(n)) {}

void typecheck::GenericType::CopyFrom(const GenericType& other) {
	if (this == &other) {
		return;
	}

	this->_name = other._name;
	this->_typeParams = other._typeParams;
}

auto typecheck::GenericType::operator==(const GenericType& other) const noexcept -> bool {
	if (this->_name != other._name) {
		return false;
	}

	if (this->_typeParams.size() != other._typeParams.size()) {
		return false;
	}

	for (std::size_t i = 0; i < this->_typeParams.size(); ++i) {
		if (!(this->_typeParams[i] == other._typeParams[i])) {
			return false;
		}
	}

	return true;
}

auto typecheck::GenericType::operator!=(const GenericType& other) const noexcept -> bool {
	return !(*this == other);
}

auto typecheck::GenericType::name() const -> std::string {
	return this->_name;
}

void typecheck::GenericType::set_name(const std::string& name) {
	this->_name = name;
}

auto typecheck::GenericType::type_params_size() const -> std::size_t {
	return this->_typeParams.size();
}

auto typecheck::GenericType::type_params(std::size_t i) const -> const Type& {
	return this->_typeParams.at(i);
}

auto typecheck::GenericType::mutable_type_params(std::size_t i) -> Type* {
	return &this->_typeParams.at(i);
}

auto typecheck::GenericType::add_type_param() -> Type* {
	this->_typeParams.emplace_back(Type{});
	return &this->_typeParams.back();
}

auto typecheck::GenericType::is_generic() const -> bool {
	return this->_typeParams.size() > 0;
}

auto typecheck::GenericType::ShortDebugString() const -> std::string {
	std::stringstream ss;
	ss << this->_name;
	
	if (this->_typeParams.size() > 0) {
		ss << "<";
		for (std::size_t i = 0; i < this->_typeParams.size(); ++i) {
			if (i > 0) {
				ss << ", ";
			}
			// For type parameters, extract the actual type name
			const auto& param = this->_typeParams[i];
			if (param.has_generic()) {
				ss << param.generic().ShortDebugString();
			} else if (param.has_func()) {
				ss << param.func().ShortDebugString();
			} else {
				ss << param.ShortDebugString();
			}
		}
		ss << ">";
	}
	
	return ss.str();
}

