#include "typecheck/ConstraintPass.hpp"
#include "typecheck/Type.hpp"
#include "typecheck/TypeVar.hpp"  // for Constraint, ConstraintKind

#include <iostream>

auto typecheck::ConstraintPass::GetResolvedType(const TypeVar& var) const -> Type {
	Type type;
    if (!this->HasResolvedType(var)) {
        std::cout << "Typecheck Error: asking for unresolved type: " << var.symbol() << std::endl;
        return type;
    }

    return this->resolvedTypes.at(var.symbol());
}

auto typecheck::ConstraintPass::HasResolvedType(const TypeVar& var) const -> bool {
    return this->resolvedTypes.find(var.symbol()) != this->resolvedTypes.end();
}

auto typecheck::ConstraintPass::SetResolvedType(const TypeVar& var, const Type& type) -> bool {
    if (!var.symbol().empty()) {
        const auto [it, didInsert] = this->resolvedTypes.emplace(var.symbol(), type);
        if (!didInsert) {
            this->resolvedTypes.at(var.symbol()) = type;
        }
        return true;
    }

    return false;
}
