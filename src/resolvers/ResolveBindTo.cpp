//
//  ResolveBindTo.cpp
//  typecheck
//
//  Created by Matthew Paletta on 2020-07-17.
//

#include "typecheck/resolvers/ResolveBindTo.hpp"
#include "typecheck/debug.hpp"

#include <google/protobuf/util/message_differencer.h>

typecheck::ResolveBindTo::ResolveBindTo(ConstraintPass* _pass, const ConstraintPass::IDType _id) : Resolver(ConstraintKind::Bind, _pass, _id) {}

auto typecheck::ResolveBindTo::clone(ConstraintPass* _pass, const ConstraintPass::IDType _id) const -> std::unique_ptr<typecheck::Resolver> {
    return std::make_unique<ResolveBindTo>(_pass, _id);
}

auto typecheck::ResolveBindTo::is_valid_constraint(const Constraint& constraint) const -> bool {
    return constraint.has_explicit_() && constraint.explicit_().has_type() && constraint.explicit_().has_var();
}

auto typecheck::ResolveBindTo::hasMoreSolutions(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) -> bool {
    return !this->has_gotten_resolve && this->is_valid_constraint(constraint);
}

auto typecheck::ResolveBindTo::readyToResolve(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> bool {
    const auto var = constraint.explicit_().var();
    const auto hasResolved = this->pass->hasResolvedType(var);
    const auto hasPermission = this->pass->HasPermission(constraint, var, manager);
    const auto canProceed = (hasResolved || hasPermission);
    const auto isValid = this->is_valid_constraint(constraint);
    return true; // isValid && canProceed;
}

auto typecheck::ResolveBindTo::resolveNext(const Constraint& constraint, const TypeManager* manager) -> bool {
    this->has_gotten_resolve = true;
    if (!this->is_valid_constraint(constraint)) {
        return false;
    }

    const auto var = constraint.explicit_().var();
    const auto type = constraint.explicit_().type();
    if (this->pass->hasResolvedType(var)) {
        // It's already been resolved, but it's the same, so we're happy
        if (google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(var), type)) {
            return true;
        }
    }

    // it's either not assigned, or it's not the same, so try and modify it.
    return this->pass->setResolvedType(constraint, var, type, manager);
}

auto typecheck::ResolveBindTo::score(const Constraint& constraint, [[maybe_unused]] const TypeManager* manager) const -> std::size_t {
    if (!this->is_valid_constraint(constraint) || !this->pass->hasResolvedType(constraint.explicit_().var())) {
        return std::numeric_limits<std::size_t>::max();
    }

    // If var is resolved, and it's type equals what it's supposed to be.
    if (google::protobuf::util::MessageDifferencer::Equals(this->pass->getResolvedType(constraint.explicit_().var()), constraint.explicit_().type())) {
        // All args found, and matched up, and return types found and match up.
        return 0;
    }

    return std::numeric_limits<std::size_t>::max();
}
