#include "typecheck/TypeManager.hpp"
#include "typecheck/Constraint.hpp"
#include "typecheck/Debug.hpp"

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
#include <iostream>
#include <string>
#endif

#include <algorithm> // for std::sort

using namespace typecheck;

namespace {
	auto getNewBlankConstraint(ConstraintKind kind, const long long& id) -> Constraint {
		Constraint constraint;
		constraint.set_kind(kind);
		constraint.set_id(id);
		return constraint;
	}

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
	auto debug_constraint_headers(const Constraint& constraint) -> std::string {
#ifdef TYPECHECK_PRINT_SHORT_DEBUG
		return constraint.ShortDebugString();
#else
		return constraint.DebugString();
#endif
	}
#endif
}

auto TypeManager::CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1) -> Constraint::IDType {
	auto constraint = getNewBlankConstraint(ConstraintKind::Equal, this->constraint_generator.next_id());

	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(!t1.symbol().empty(), "Cannot use empty type when creating constraint.");

	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t1.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_types()->mutable_first()->CopyFrom(t0);
	constraint.mutable_types()->mutable_second()->CopyFrom(t1);

	// If both type variables are arrays, also create an Equals constraint between their element types
	auto t0ElementIt = this->arrayElementMap.find(t0.symbol());
	auto t1ElementIt = this->arrayElementMap.find(t1.symbol());
	
	if (t0ElementIt != this->arrayElementMap.end() && t1ElementIt != this->arrayElementMap.end()) {
		// Both are arrays, create element equality
		TypeVar t0Element;
		t0Element.set_symbol(t0ElementIt->second);
		TypeVar t1Element;
		t1Element.set_symbol(t1ElementIt->second);
		
		// Recursively create equals constraint for elements
		// Note: We need to add this constraint first, then add the main constraint
		// to ensure proper constraint ordering
		auto elementConstraint = getNewBlankConstraint(ConstraintKind::Equal, this->constraint_generator.next_id());
		elementConstraint.mutable_types()->mutable_first()->CopyFrom(t0Element);
		elementConstraint.mutable_types()->mutable_second()->CopyFrom(t1Element);
		
#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
		std::cout << "Auto-generated element equality: " << debug_constraint_headers(elementConstraint) << std::endl;
#endif
		this->constraints.emplace_back(elementConstraint);
	}

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

auto TypeManager::CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind::LiteralProtocol& protocol) -> Constraint::IDType {
	auto constraint = getNewBlankConstraint(ConstraintKind::ConformsTo, this->constraint_generator.next_id());

	TYPECHECK_ASSERT(!t0.symbol().empty(), "Cannot use empty type when creating constraint.");
	TYPECHECK_ASSERT(this->registeredTypeVars.find(t0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

	constraint.mutable_conforms()->mutable_type()->CopyFrom(t0);
	constraint.mutable_conforms()->mutable_protocol()->set_literal(protocol);


#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

	this->constraints.emplace_back(constraint);
	return constraint.id();
}

auto TypeManager::CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1) -> Constraint::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::Conversion, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    TYPECHECK_ASSERT(!T1.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T1.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    constraint.mutable_types()->mutable_first()->CopyFrom(T0);
    constraint.mutable_types()->mutable_second()->CopyFrom(T1);

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

auto TypeManager::CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const std::vector<Type>& args, const Type& return_type) -> Constraint::IDType {
    const auto returnVar = this->CreateTypeVar();
    std::vector<TypeVar> argVars;
    for (std::size_t i = 0; i < args.size(); ++i) {
        argVars.emplace_back(this->CreateTypeVar());
    }
    const auto constraintID = this->CreateApplicableFunctionConstraint(functionid, argVars, returnVar);

    // Wait until after we created the constraint before creating constraints, othersise, could have loose ends
    this->CreateBindToConstraint(returnVar, return_type);
    for (std::size_t i = 0; i < args.size(); ++i) {
        this->CreateBindToConstraint(argVars.at(i), args.at(i));
    }

    return constraintID;
}

auto TypeManager::CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const std::vector<TypeVar>& argVars, const TypeVar& returnTypeVar) -> Constraint::IDType {
    FunctionVar funcVar;
    funcVar.set_id(functionid);
    for (const auto& arg : argVars) {
        funcVar.add_args()->CopyFrom(arg);
    }

    funcVar.mutable_returnvar()->CopyFrom(returnTypeVar);
    return this->CreateApplicableFunctionConstraint(functionid, funcVar);
}

auto TypeManager::CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const FunctionVar& type) -> Constraint::IDType {
    TYPECHECK_ASSERT(type.id() == functionid, "Function type ID should match function id and be set.");

    this->functions.push_back(type);
    return type.id();
}

auto TypeManager::CreateBindFunctionConstraint(const Constraint::IDType& functionid, const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType) -> Constraint::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::BindOverload, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    constraint.mutable_overload()->mutable_type()->CopyFrom(T0);
    constraint.mutable_overload()->set_functionid(functionid);

    for (const auto& arg : args) {
        TYPECHECK_ASSERT(!arg.symbol().empty(), "Cannot use empty type when creating constraint.");
        TYPECHECK_ASSERT(this->registeredTypeVars.find(arg.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
        constraint.mutable_overload()->add_argvars()->CopyFrom(arg);
    }

    TYPECHECK_ASSERT(!returnType.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(returnType.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    constraint.mutable_overload()->mutable_returnvar()->CopyFrom(returnType);

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

auto TypeManager::CreateBindToConstraint(const TypeVar& T0, const Type& type) -> Constraint::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::Bind, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!T0.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(T0.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    TYPECHECK_ASSERT(type.has_generic() || type.has_func(), "Must insert valid type.");

    constraint.mutable_explicit()->mutable_var()->CopyFrom(T0);
    constraint.mutable_explicit()->mutable_type()->CopyFrom(type);

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}

auto TypeManager::CreateArrayElementConstraint(const TypeVar& arrayVar, const TypeVar& elementVar) -> Constraint::IDType {
    auto constraint = getNewBlankConstraint(ConstraintKind::ArrayElement, this->constraint_generator.next_id());

    TYPECHECK_ASSERT(!arrayVar.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(arrayVar.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");
    TYPECHECK_ASSERT(!elementVar.symbol().empty(), "Cannot use empty type when creating constraint.");
    TYPECHECK_ASSERT(this->registeredTypeVars.find(elementVar.symbol()) != this->registeredTypeVars.end(), "Must create type var before using.");

    constraint.mutable_types()->mutable_first()->CopyFrom(arrayVar);
    constraint.mutable_types()->mutable_second()->CopyFrom(elementVar);

    // Track the array-element relationship
    this->arrayElementMap[arrayVar.symbol()] = elementVar.symbol();

#ifdef TYPECHECK_PRINT_DEBUG_CONSTRAINTS
    std::cout << debug_constraint_headers(constraint) << std::endl;
#endif

    this->constraints.emplace_back(constraint);
    return constraint.id();
}
