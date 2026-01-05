#include "typecheck/Type.hpp"
#include "typecheck/FunctionDefinition.hpp"
#include "typecheck/GenericType.hpp"

#include <iostream>
#include <sstream>
#include <variant>

typecheck::Type::Type() : data(false) {}

typecheck::Type::Type(const GenericType& g) : data(g) {}

typecheck::Type::Type(const FunctionDefinition& f) : data(f) {}

typecheck::Type::Type(const Type& other)  = default;

auto typecheck::Type::operator=(const Type& other) -> Type& {
	if (this == &other) {
		return *this;
	}

	this->CopyFrom(other);
	return *this;
}

typecheck::Type::Type(Type&& other) noexcept : data(std::move(other.data)) {}

auto typecheck::Type::operator=(Type&& other) noexcept -> Type& {
	if (this == &other) {
		return *this;
	}

	this->data = std::move(other.data);

	return *this;
}

auto typecheck::Type::operator==(const Type& other) const noexcept -> bool {
	if (this->has_generic() && other.has_generic()) {
		return this->generic() == other.generic();
	}

	if (this->has_func() && other.has_func()) {
		return this->func() == other.func();
	}

	return !this->has_generic() && !other.has_generic() && !this->has_func() && !other.has_func();
}

auto typecheck::Type::CopyFrom(const Type& other) -> Type& {
	if (this == &other) {
		return *this;
	}

	if (other.has_generic()) {
		this->mutable_generic()->CopyFrom(other.generic());
	} else if (other.has_func()) {
		this->mutable_func()->CopyFrom(other.func());
	} else {
		this->data.emplace<bool>(false);
	}

	return *this;
}

auto typecheck::Type::has_generic() const -> bool {
	return std::holds_alternative<GenericType>(this->data);
}

auto typecheck::Type::has_func() const -> bool {
	return std::holds_alternative<FunctionDefinition>(this->data);
}

auto typecheck::Type::mutable_generic() -> GenericType* {
	if (!this->has_generic()) {
		this->data = GenericType{};
	}
	return std::get_if<GenericType>(&this->data);
}

auto typecheck::Type::generic() const -> const GenericType& {
	if (!this->has_generic()) {
		this->data = GenericType{};
	}
	return std::get<GenericType>(this->data);
}

auto typecheck::Type::mutable_func() -> FunctionDefinition* {
	if (!this->has_func()) {
		this->data = FunctionDefinition{};
	}
	return std::get_if<FunctionDefinition>(&this->data);
}

auto typecheck::Type::func() const -> const FunctionDefinition& {
	if (!this->has_func()) {
		this->data = FunctionDefinition{};
	}
	return std::get<FunctionDefinition>(this->data);
}

auto typecheck::Type::ShortDebugString() const -> std::string {
	std::stringstream out;
	out << "{ ";
	if (this->has_generic()) {
		out << "\"generic\": " << this->generic().ShortDebugString() << (this->has_func() ? ", " : " ");
	}
	if (this->has_func()) {
		out << "\"func\": " << this->func().ShortDebugString() << " ";
	}
	out << "}";
	return out.str();
}
