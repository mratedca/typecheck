#include "typecheck/FunctionDefinition.hpp"
#include "typecheck/Type.hpp"
#include <sstream>

typecheck::FunctionDefinition::FunctionDefinition() noexcept : _id(0) {}

typecheck::FunctionDefinition::FunctionDefinition(const FunctionDefinition& other) : _id(0) {
	this->CopyFrom(other);
}

auto typecheck::FunctionDefinition::operator=(const FunctionDefinition& other) -> FunctionDefinition& {
	if (this == &other) {
		return *this;
	}

	this->CopyFrom(other);
	return *this;
}

void typecheck::FunctionDefinition::CopyFrom(const FunctionDefinition& other) {
	this->_args = other._args;

	if (other.has_returntype()) {
		this->_returnType = std::make_unique<Type>();
		this->_returnType->CopyFrom(other.returntype());
	}

	this->_name = other._name;
	this->_id = other._id;
}

auto typecheck::FunctionDefinition::operator==(const FunctionDefinition& other) const noexcept -> bool {
	const auto args_same = this->args_size() == other.args_size() &&
		this->name() == other.name() &&
		this->id() == other.id() &&
		this->has_returntype() == other.has_returntype() &&
		// This is the most expensive, do it last
		this->_args == other._args;

	if (args_same && this->has_returntype() && other.has_returntype()) {
		// Break ties by checking return Type, not the unique_ptr
		return this->returntype() == other.returntype();
	}
	
	return args_same;
}

auto typecheck::FunctionDefinition::operator!=(const FunctionDefinition& other) const noexcept -> bool {
	return !(*this == other);
}

auto typecheck::FunctionDefinition::mutable_returntype() -> Type* {
	if (!this->has_returntype()) {
		this->_returnType = std::make_unique<Type>();
	}
	return this->_returnType.get();
}

auto typecheck::FunctionDefinition::has_returntype() const -> bool {
	return this->_returnType.operator bool();
}

auto typecheck::FunctionDefinition::returntype() const -> const Type& {
	if (!this->has_returntype()) {
		this->_returnType = std::make_unique<Type>();
	}
	return *this->_returnType;
}

auto typecheck::FunctionDefinition::add_args() -> Type* {
	this->_args.emplace_back();
	return &this->_args.at(this->_args.size() - 1);
}

auto typecheck::FunctionDefinition::args_size() const -> std::size_t {
	return this->_args.size();
}

auto typecheck::FunctionDefinition::args(std::size_t i) const -> const Type& {
	return this->_args.at(i);
}

auto typecheck::FunctionDefinition::name() const -> std::string {
	return this->_name;
}

void typecheck::FunctionDefinition::set_name(const std::string& name) {
    this->_name = name;
}

auto typecheck::FunctionDefinition::id() const -> long long {
	return this->_id;
}

void typecheck::FunctionDefinition::set_id(long long id) {
    this->_id = id;
}

std::string typecheck::FunctionDefinition::ShortDebugString() const {
	std::stringstream out;
	out << "{ ";
	out << "\"name\": \"" << this->_name << "\", ";
	if (this->has_returntype()) {
		out << "\"returnType\": \"" << this->returntype().ShortDebugString() << "\", ";
	}
	out << "\"id\": " << this->_id << ", ";
	out << "\"args\": [";
	for (const auto& arg : this->_args) {
		out << arg.ShortDebugString() << (!(arg == this->args(this->args_size() - 1)) ? ", " : " ");
	}
	out << "],";
	out << " }";
	return out.str();
}
