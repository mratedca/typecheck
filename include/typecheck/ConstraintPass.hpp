#pragma once

#include "TypeVar.hpp"
#include "Type.hpp"

#include <memory>                            // for unique_ptr
#include <string>
#include <unordered_map>                     // for unordered_map


namespace typecheck {
	class ConstraintPass {
    public:
		ConstraintPass() = default;
		~ConstraintPass() = default;

		[[nodiscard]] auto GetResolvedType(const TypeVar& var) const -> Type;
		[[nodiscard]] auto HasResolvedType(const TypeVar& var) const -> bool;
		auto SetResolvedType(const TypeVar& var, const Type& type) -> bool;

	private:
        // The key must be string, because 'typeVar' not comparable.
        std::unordered_map<std::string, Type> resolvedTypes;
	};
}
