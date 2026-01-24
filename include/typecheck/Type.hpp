#pragma once

#include "GenericType.hpp"
#include "FunctionDefinition.hpp"

#include <variant>

namespace typecheck {
	class Type {
	public:
		Type();
		Type(const GenericType& g);
		Type(const FunctionDefinition& f);
		~Type() = default;

		Type(const Type& other);
		auto operator=(const Type& other) -> Type&;
		Type(Type&& other) noexcept;
		auto operator=(Type&& other) noexcept -> Type&;

		auto operator==(const Type& other) const noexcept -> bool;
		auto operator!=(const Type& other) const noexcept -> bool {
			return !(*this == other);
		}

		auto CopyFrom(const Type& other) -> Type&;

		auto has_generic() const -> bool;
		auto has_func() const -> bool;

		auto mutable_generic() -> GenericType*;
		auto generic() const -> const GenericType&;

		auto mutable_func() -> FunctionDefinition*;
		auto func() const -> const FunctionDefinition&;

		auto ShortDebugString() const -> std::string;
	private:
		// Variant defaults to the first type
		mutable std::variant<bool, GenericType, FunctionDefinition> data;
	};
}
