#pragma once

#include "RawType.hpp"
#include "FunctionDefinition.hpp"

#include <variant>

namespace typecheck {
	class Type {
	public:
		Type();
		Type(const RawType& r);
		Type(const FunctionDefinition& f);
		~Type() = default;

		Type(const Type& other);
		auto operator=(const Type& other) -> Type&;
		Type(Type&& other) noexcept;
		auto operator=(Type&& other) noexcept -> Type&;

		auto operator==(const Type& other) const noexcept -> bool;

		auto CopyFrom(const Type& other) -> Type&;

		auto has_raw() const -> bool;
		auto has_func() const -> bool;

		auto mutable_raw() -> RawType*;
		auto raw() const -> const RawType&;

		auto mutable_func() -> FunctionDefinition*;
		auto func() const -> const FunctionDefinition&;

		auto ShortDebugString() const -> std::string;
	private:
		// Variant defaults to the first type
		mutable std::variant<bool, RawType, FunctionDefinition> data;
	};
}
