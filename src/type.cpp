#include "typecheck/Type.hpp"
#include "typecheck/FunctionDefinition.hpp"
#include "typecheck/RawType.hpp"

#include <iostream>
#include <sstream>
#include <variant>

typecheck::Type::Type() : data(false) {}

typecheck::Type::Type(const RawType& r) : data(r) {}

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
	if (this->has_raw() && other.has_raw()) {
		return this->raw() == other.raw();
	}

	if (this->has_func() && other.has_func()) {
		return this->func() == other.func();
	}

	return !this->has_raw() && !other.has_raw() && !this->has_func() && !other.has_func();
}

auto typecheck::Type::CopyFrom(const Type& other) -> Type& {
	if (this == &other) {
		return *this;
	}

	if (other.has_raw()) {
		this->mutable_raw()->CopyFrom(other.raw());
	} else if (other.has_func()) {
		this->mutable_func()->CopyFrom(other.func());
	} else {
		this->data.emplace<bool>(false);
	}

	return *this;
}

auto typecheck::Type::has_raw() const -> bool {
	return std::holds_alternative<RawType>(this->data);
}

auto typecheck::Type::has_func() const -> bool {
	return std::holds_alternative<FunctionDefinition>(this->data);
}

auto typecheck::Type::mutable_raw() -> RawType* {
	if (!this->has_raw()) {
		this->data = RawType{};
	}
	return std::get_if<RawType>(&this->data);
}

auto typecheck::Type::raw() const -> const RawType& {
	if (!this->has_raw()) {
		this->data = RawType{};
	}
	return std::get<RawType>(this->data);
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
	if (this->has_raw()) {
		out << "\"raw\": " << this->raw().ShortDebugString() << (this->has_func() ? ", " : " ");
	}
	if (this->has_func()) {
		out << "\"func\": " << this->func().ShortDebugString() << " ";
	}
	out << "}";
	return out.str();
}
