#pragma once

#include "Constraint.hpp"
#include "ConstraintPass.hpp"
#include "FunctionVar.hpp"
#include "GenericTypeGenerator.hpp"

#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace typecheck {

	class TypeManager {
	public:
		TypeManager();
		~TypeManager() = default;

		// Not moveable or copyable
		TypeManager(const TypeManager&) = delete;
		auto operator=(const TypeManager&) -> TypeManager& = delete;
		TypeManager(TypeManager&&) = delete;
		auto operator=(TypeManager&&) -> TypeManager& = delete;

		// 'Register' types
        auto registerType(const std::string& name) -> bool;
		auto registerType(const Type& name) -> bool;
        [[nodiscard]] auto hasRegisteredType(const std::string& name) const noexcept -> bool;
		[[nodiscard]] auto hasRegisteredType(const Type& name) const noexcept -> bool;
        [[nodiscard]] auto getRegisteredType(const std::string& name) const noexcept -> Type;
		[[nodiscard]] auto getRegisteredType(const Type& name) const noexcept -> Type;

		// 'Register' convertible types
        auto setConvertible(const std::string& T0, const std::string& T1) -> bool;
		auto setConvertible(const Type& T0, const Type& T1) -> bool;
        [[nodiscard]] auto isConvertible(const std::string& T0, const std::string& T1) const noexcept -> bool;
		[[nodiscard]] auto isConvertible(const Type& T0, const Type& T1) const noexcept -> bool;
        [[nodiscard]] auto getConvertible(const Type& T0) const -> std::vector<Type>;

		auto CreateTypeVar() -> const typecheck::TypeVar;
		[[nodiscard]] auto CreateFunctionHash(const std::string& name, const std::vector<std::string>& argNames) const -> Constraint::IDType;
		[[nodiscard]] auto CreateLambdaFunctionHash(const std::vector<std::string>& argNames) const -> Constraint::IDType;
        auto CreateLiteralConformsToConstraint(const TypeVar& t0, const KnownProtocolKind::LiteralProtocol& protocol) -> Constraint::IDType;
        auto CreateEqualsConstraint(const TypeVar& t0, const TypeVar& t1) -> Constraint::IDType;
        auto CreateConvertibleConstraint(const TypeVar& T0, const TypeVar& T1) -> Constraint::IDType;
        auto CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const std::vector<Type>& args, const Type& return_type) -> Constraint::IDType;
        auto CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const std::vector<TypeVar>& argVars, const TypeVar& returnTypeVar) -> Constraint::IDType;
        auto CreateApplicableFunctionConstraint(const Constraint::IDType& functionid, const FunctionVar& type) -> Constraint::IDType;
        auto CreateBindFunctionConstraint( const Constraint::IDType& functionid, const TypeVar& T0, const std::vector<TypeVar>& args, const TypeVar& returnType) -> Constraint::IDType;
        auto CreateBindToConstraint(const typecheck::TypeVar& T0, const typecheck::Type& type) -> Constraint::IDType;

        [[nodiscard]] auto getConstraint(Constraint::IDType id) const -> const Constraint*;

		auto solve() -> std::optional<ConstraintPass>;
		std::vector<Constraint> constraints;

	private:
		std::vector<Type> registeredTypes;
		std::set<std::string> registeredTypeVars;
		std::map<std::string, std::set<std::string>> convertible;
		std::vector<FunctionVar> functions;

		GenericTypeGenerator type_generator;
		GenericTypeGenerator constraint_generator;

        [[nodiscard]] auto getFunctionOverloads(Constraint::IDType funcID) const -> std::vector<FunctionVar>;

        // Internal helper
        auto getConstraintInternal(Constraint::IDType id) -> Constraint*;
	};
}
