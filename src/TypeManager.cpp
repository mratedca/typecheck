#include "typecheck/TypeManager.hpp"
#include "typecheck/Constraint.hpp"           // for ConstraintKind
#include "typecheck/Debug.hpp"
#include "typecheck/GenericTypeGenerator.hpp"       // for GenericTypeGene...
#include "typecheck/Type.hpp"                 // for Type, TypeVar
#include "typecheck/protocols/ExpressibleByDoubleLiteral.hpp"
#include "typecheck/protocols/ExpressibleByFloatLiteral.hpp"
#include "typecheck/protocols/ExpressibleByIntegerLiteral.hpp"

#include "constraint/Domain.hpp"
#include "constraint/Env.hpp"
#include "constraint/Node.hpp"
#include "constraint/Solver.hpp"
#include "constraint/State.hpp"
#include "constraint/StateQuery.hpp"

#include "cppnotstdlib/strings.hpp"

#include <cassert>
#include <iostream>
#include <limits>                                     // for numeric_limits
#include <list>
#include <optional>
#include <queue>
#include <sstream>                                    // for std::stringstream
#include <stdexcept>
#include <string>                                     // for std::string
#include <type_traits>                                // for move
#include <utility>                                    // for make_pair

typecheck::TypeManager::TypeManager() = default;

auto typecheck::TypeManager::registerType(const std::string& name) -> bool {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->registerType(ty);
}

auto typecheck::TypeManager::registerType(const Type& name) -> bool {
	// Determine if has type
	const auto alreadyHasType = this->hasRegisteredType(name);
	if (!alreadyHasType) {
		Type type;
		type.CopyFrom(name);
		this->registeredTypes.emplace_back(type);
	}
	return !alreadyHasType;
}

auto typecheck::TypeManager::hasRegisteredType(const std::string& name) const noexcept -> bool {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

auto typecheck::TypeManager::hasRegisteredType(const Type& name) const noexcept -> bool {
    const auto returned = this->getRegisteredType(name);
    return returned.has_raw() || returned.has_func();
}

auto typecheck::TypeManager::getRegisteredType(const std::string& name) const noexcept -> Type {
    Type ty;
    ty.mutable_raw()->set_name(name);
    return this->getRegisteredType(ty);
}

auto typecheck::TypeManager::getRegisteredType(const Type& name) const noexcept -> Type {
	for (const auto& type : this->registeredTypes) {
        if (type == name) {
			return type;
		}
	}

	return {};
}

auto typecheck::TypeManager::getFunctionOverloads(Constraint::IDType funcID) const -> std::vector<FunctionVar> {
    std::vector<FunctionVar> overloads;
    for (const auto& overload : this->functions) {
        // Lookup by 'var', to deal with anonymous functions.
        if (overload.id() == funcID) {
            // Copy it over, and hand it over a 'function definition'.

            overloads.push_back(overload);
        }
    }

    return overloads;
}

auto typecheck::TypeManager::setConvertible(const std::string& T0, const std::string& T1) -> bool {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);

    return this->setConvertible(t0, t1);
}

auto typecheck::TypeManager::CreateFunctionHash(const std::string& name, const std::vector<std::string>& argNames) const -> Constraint::IDType {
    return static_cast<Constraint::IDType>(std::hash<std::string>()(name + cppnotstdlib::string::join(argNames, ":")));
}

auto typecheck::TypeManager::CreateLambdaFunctionHash(const std::vector<std::string>& argNames) const -> Constraint::IDType {
    // Lambda functions use the address of the arguments as part of the name
    const void* address = static_cast<const void*>(&argNames);
    std::stringstream ss;
    ss << address;
    std::string lambdaAddress = ss.str();

    return this->CreateFunctionHash("lambda" + lambdaAddress, argNames);
}

auto typecheck::TypeManager::setConvertible(const Type& T0, const Type& T1) -> bool {
    if (T0 == T1) {
		return true;
	}

	const auto& t0_ptr = this->getRegisteredType(T0);
	const auto& t1_ptr = this->getRegisteredType(T1);

    // Function types not convertible
    if (t0_ptr.has_func() || t1_ptr.has_func()) {
        // Functions not convertible to each other
        return false;
    } else if (!t0_ptr.raw().name().empty() && !t1_ptr.raw().name().empty() && this->convertible[t0_ptr.raw().name()].find(t1_ptr.raw().name()) == this->convertible[t0_ptr.raw().name()].end()) {
		// Convertible from T0 -> T1
        this->convertible[t0_ptr.raw().name()].insert(t1_ptr.raw().name());
		return true;
	}
	return false;
}

auto typecheck::TypeManager::isConvertible(const std::string& T0, const std::string& T1) const noexcept -> bool {
    Type t0;
    t0.mutable_raw()->set_name(T0);

    Type t1;
    t1.mutable_raw()->set_name(T1);
    return this->isConvertible(t0, t1);
}

auto typecheck::TypeManager::isConvertible(const Type& T0, const Type& T1) const noexcept -> bool {
    if (T0.raw().name().empty() || T1.raw().name().empty()) {
        // Undefined types, stop here.
        return false;
    }

    // Function types not convertible
    if (T0.has_func() || T1.has_func()) {
        return false;
    }

    if (T0 == T1) {
		return true;
	}

    // Because they're not functions, they must both be raw.
    if (this->convertible.find(T0.raw().name()) == this->convertible.end()) {
		// T0 is not in the map, meaning the conversion won't be there.
		return false;
	}

    if (this->convertible.at(T0.raw().name()).find(T1.raw().name()) != this->convertible.at(T0.raw().name()).end()) {
		// Convertible from T0 -> T1
		return true;
	}
	return false;
}

auto typecheck::TypeManager::getConvertible(const Type& T0) const -> std::vector<Type> {
    std::vector<Type> out;

    // Function types not convertible
    if (T0.has_func()) {
        return out;
    }

    if (this->convertible.find(T0.raw().name()) != this->convertible.end()) {
        // Load into vector
        for (const auto& convert : this->convertible.at(T0.raw().name())) {
            Type type;
            type.mutable_raw()->set_name(convert);
            out.emplace_back(std::move(type));
        }
    }

    return out;
}

auto typecheck::TypeManager::CreateTypeVar() -> const TypeVar {
	const auto var = this->type_generator.next();

	this->registeredTypeVars.insert(var);

	TypeVar type;
	type.set_symbol(var);
	return type;
}

auto typecheck::TypeManager::getConstraintInternal(const Constraint::IDType id) -> Constraint* {
    for (auto& constraint : this->constraints) {
        if (constraint.id() == id) {
            return &constraint;
        }
    }

    return nullptr;
}

auto typecheck::TypeManager::getConstraint(const Constraint::IDType id) const -> const Constraint* {
	for (const auto& constraint : this->constraints) {
		if (constraint.id() == id) {
			return &constraint;
		}
	}

	return nullptr;
}

namespace {
    typecheck::Type TypeFromString(const std::string& val, const constraint::Solution& sol) {
        if (cppnotstdlib::string::explode(val, '|').size() == 1) {
            return {typecheck::RawType(val)};
        } else {
            const auto fvar = typecheck::FunctionVar::unserialize(val);
            typecheck::FunctionDefinition funcDef;
            funcDef.set_name(fvar.name());
            funcDef.set_id(fvar.id());

            const auto returnVar = fvar.returnvar().symbol();
            const auto lookupReturnVar = sol.At(returnVar).to_string();

            // A function should not return itself.
            // Prevent infinite loops
            assert(returnVar != val);
            assert(lookupReturnVar != val);

            funcDef.mutable_returntype()->CopyFrom(TypeFromString(lookupReturnVar, sol));
            for (const auto& a : fvar.args()) {
                const auto foundVariable = a.symbol(); 
                const auto resolvedVariable = sol.At(foundVariable).to_string();

                // Prevent infinite loops.
                assert(foundVariable != val);

                if (!sol.Contains(foundVariable)) {
                    throw std::logic_error("Solution does not contain variable");
                }

                const auto foundType = TypeFromString(resolvedVariable, sol);
                funcDef.add_args()->CopyFrom(foundType);
            }
            return {funcDef};
        }
    }

    void AddTypeToDomain(constraint::Domain::data_type& domain, const typecheck::Type& type) {
        if (type.has_raw()) {
            domain.emplace_back(type.raw().name());
        } else if (type.has_func()) {
            domain.emplace_back(type.func().name());
        }
    }

    void AddTypeToDomain(constraint::Domain::data_type& domain, const typecheck::FunctionVar& type) {
        domain.emplace_back(type.serialize());
    }

    using distance_type = std::function<std::size_t(const constraint::State&)>;

    template<typename T>
    void AddLiteralProtocolTypes(constraint::Domain::data_type& domain) {
        T protocol;
        for (const auto& ty : protocol.getPreferredTypes()) {
            AddTypeToDomain(domain, ty);
        }

        for (const auto& ty : protocol.getOtherTypes()) {
            AddTypeToDomain(domain, ty);
        }
    }

    template<typename T>
    void AddHeuristicProtocolFuncs(std::vector<constraint::Solver::DistanceFunc>& heuristics, std::vector<constraint::Solver::DistanceFunc>& actuals, const std::string& var) {
        heuristics.emplace_back([var](const constraint::StateQuery& state) {
            T protocol;
            if (state.IsAssigned(var)) {
                // Check if in preferred list or not.
                for (const auto& ty : protocol.getPreferredTypes()) {
                    if (ty.raw().name() == state.variable_map.at(var).to_string()) {
                        return 0;
                    }
                }

                return 1;
            }

            // Unknown.
            return 2;
        });

        // Use the same one for the actual solution, except punish more for not assigned.
        actuals.emplace_back([var](const constraint::StateQuery& state) {
            T protocol;
            if (state.IsAssigned(var)) {
                // Check if in preferred list or not.
                for (const auto& ty : protocol.getPreferredTypes()) {
                    if (ty.raw().name() == state.variable_map.at(var).to_string()) {
                        return 0;
                    }
                }

                return 1;
            }

            // Unknown.
            return 2;
        });
    }
}

auto typecheck::TypeManager::solve() -> std::optional<ConstraintPass> {
    constraint::Solver constraint_solver;
    std::set<std::string> all_variable_names;

    std::vector<constraint::Solver::DistanceFunc> heuristcFuncs;
    std::vector<constraint::Solver::DistanceFunc> distanceFuncs;

#pragma mark - Gather All Data
    auto insert_if_not_exists = [&constraint_solver, &all_variable_names](const std::string& var, const constraint::Domain& domain) {
        if (domain.size() == 0) {
            std::cout << "Warning: Domain Empty for variable: " << var << std::endl;
        }

        if (all_variable_names.find(var) == all_variable_names.end()) {
            constraint_solver.AddVariable(var, domain);
            all_variable_names.insert(var);
        }
    };

    // Var Domain
    const auto varDomain = [this] {
        constraint::Domain::data_type domain;
        for (const auto& ty : this->registeredTypes) {
            AddTypeToDomain(domain, ty);
        }

        for (const auto& func : this->functions) {
            AddTypeToDomain(domain, func);
        }

        return constraint::Domain(domain);
    }();

    for (const auto& constraint : this->constraints) {
        if (constraint.has_conforms()) {
            const auto conforms = constraint.conforms();
            if (conforms.has_type() && conforms.has_protocol()) {
                const auto var = conforms.type().symbol();
                const auto protocol = conforms.protocol();
                constraint::Domain::data_type domain;
                switch (protocol.literal()) {
                case KnownProtocolKind::ExpressibleByFloat:
                    AddLiteralProtocolTypes<ExpressibleByFloatLiteral>(domain);
                    AddHeuristicProtocolFuncs<ExpressibleByFloatLiteral>(heuristcFuncs, distanceFuncs, var);
                    break;
                case KnownProtocolKind::ExpressibleByDouble:
                    AddLiteralProtocolTypes<ExpressibleByDoubleLiteral>(domain);
                    AddHeuristicProtocolFuncs<ExpressibleByDoubleLiteral>(heuristcFuncs, distanceFuncs, var);
                    break;
                case KnownProtocolKind::ExpressibleByInteger:
                    AddLiteralProtocolTypes<ExpressibleByIntegerLiteral>(domain);
                    AddHeuristicProtocolFuncs<ExpressibleByIntegerLiteral>(heuristcFuncs, distanceFuncs, var);
                    break;
				case KnownProtocolKind::ExpressibleByArray:
				case KnownProtocolKind::ExpressibleByBoolean:
				case KnownProtocolKind::ExpressibleByDictionary:
				case KnownProtocolKind::ExpressibleByNil:
				case KnownProtocolKind::ExpressibleByString:
                default:
                    std::cout << "Unsupported Literal" << std::endl;
                    return std::nullopt;
                    break;
                }
                insert_if_not_exists(var, varDomain);

                // conforms literal is implied by its domain.
                constraint_solver.AddConstraint(std::vector{var}, [var, domain](const constraint::Env& env) {
                    for (const auto& ty : domain) {
                        if (env.At(var) == ty) {
                            return true;
                        }
                    }

                    return false;
                });
            } else {
                std::cout << "Malformed Conforms Constraint" << std::endl;
                return std::nullopt;
            }
        } else if (constraint.has_types()) {
            const auto types = constraint.types();
            std::vector<std::string> type_names;
            if (types.has_first()) {
                type_names.push_back(types.first().symbol());
            }
            if (types.has_second()) {
                type_names.push_back(types.second().symbol());
            }
            if (types.has_third()) {
                type_names.push_back(types.third().symbol());
            }

            if (type_names.empty()) {
                std::cout << "Malformed Types Constraint" << std::endl;
            } else {
                for (const auto& ty : type_names) {
                    insert_if_not_exists(ty, varDomain);
                }

                switch (constraint.kind()) {
                case Conversion:
                    constraint_solver.AddConstraint(std::vector{type_names}, [type_names, C = &convertible](const constraint::Env& env) {
                        const auto firstVarValue = env.At(type_names.at(0));
                        const auto secondVarValue = env.At(type_names.at(1));

                        if (firstVarValue == secondVarValue) {
                            return true;
                        }

                        const auto it = C->find(firstVarValue.to_string());
                        if (it == C->end()) {
                            return false;
                        }

                        return it->second.find(secondVarValue.to_string()) != it->second.end();
                    });
                    break;
                case Equal:
                    constraint_solver.AddConstraint(std::vector{type_names}, [type_names](const constraint::Env& env) {
                        const auto firstVar = env.At(type_names.at(0));
                        for (const auto& ty : type_names) {
                            if (firstVar != env.At(ty)) {
                                return false;
                            }
                        }

                        return true;
                    });
                    break;
				case Bind:
				case BindParam:
				case BindOverload:
				case ConformsTo:
				case ApplicableFunction:
                default:
                    std::cout << "Unimplemented Constraint Kind: " << constraint.kind() << std::endl;
                    assert(false);
                    break;
                }
            }
        } else if (constraint.has_overload()) {
            const auto overload = constraint.overload();

            std::vector<std::string> overloadVariables;
            overloadVariables.emplace_back(overload.type().symbol());
            overloadVariables.emplace_back(overload.returnvar().symbol());
            for (std::size_t i = 0; i < overload.argvars_size(); ++i) {
                overloadVariables.emplace_back(overload.argvars(i).symbol());
            }

            // Gather all overloads.
            const auto funcFamily = this->getFunctionOverloads(overload.functionid());
            std::vector<std::vector<std::string>> all_func_dependant_variables;
            constraint::Domain::data_type typeDomain;
            for (const auto& func : funcFamily) {
                std::vector<std::string> funcDependantVariables;
                funcDependantVariables.emplace_back(func.returnvar().symbol());
                for (const auto& arg : func.args()) {
                    funcDependantVariables.emplace_back(arg.symbol());
                }

                all_func_dependant_variables.emplace_back(funcDependantVariables);
                typeDomain.emplace_back(func.serialize());
            }

            insert_if_not_exists(overload.type().symbol(), typeDomain);
            for (const auto& a : overloadVariables) {
                insert_if_not_exists(a, varDomain);
            }
            for (const auto& a : all_func_dependant_variables) {
                for (const auto& b : a) {
                    insert_if_not_exists(b, varDomain);
                }
            }


            for (std::size_t i = 0; i < funcFamily.size(); ++i) {
                const auto& vars = all_func_dependant_variables.at(i);
                const auto& func = funcFamily.at(i);

                std::vector<std::string> overloadConstraintVars;

                // Copy the variables from the overload constraint
                std::copy(overloadVariables.begin(), overloadVariables.end(), std::back_inserter(overloadConstraintVars));

                // Copy the variables from the function definition
                std::copy(vars.begin(), vars.end(), std::back_inserter(overloadConstraintVars));

                auto allFuncDefinitionVariablesAssigned = [vars](const constraint::Env& env) {
                    for (const auto& a : vars) {
                        if (!env.IsAssigned(a)) {
                            return false;
                        }
                    }
                    return true;
                };

                constraint_solver.AddConstraint(overloadConstraintVars, [overload, funcDefinition = func, check = std::move(allFuncDefinitionVariablesAssigned)](const constraint::Env& env) {
                    if (!check(env)) {
                        // If not all the variables of the function are assigned, say it's fine, and the other one will pick it up.
                        return true;
                    }

                    if (env.At(overload.type().symbol()).to_string() != funcDefinition.serialize()) {
                        // This is not the overload we are looking for.
                        return true;
                    }

                    auto compare_vars = [&env](const TypeVar& vA, const TypeVar& vB) {
                        return env.At(vA.symbol()).to_string() == env.At(vB.symbol()).to_string();
                    };

                    // This is the the overload, check everything matches up.
                    if (overload.argvars_size() != funcDefinition.args().size()) {
                        return false;
                    }

                    if (!compare_vars(overload.returnvar(), funcDefinition.returnvar())) {
                        return false;
                    }

                    for (std::size_t i = 0; i < funcDefinition.args().size(); ++i) {
                        if (!compare_vars(overload.argvars(i), funcDefinition.args().at(i))) {
                            return false;
                        }
                    }

                    return true;
                });
            }

        } else if (constraint.has_explicit()) {
            const auto explicit_ = constraint.explicit_();
            if (explicit_.has_var() && explicit_.has_type()) {
                const auto& var = explicit_.var();
                const auto& type = explicit_.type();

                // The domain can only the be the explicit type.
                if (type.has_raw()) {
                    insert_if_not_exists(var.symbol(), {type.raw().name()});
                } else if (type.has_func()) {
                    insert_if_not_exists(var.symbol(), {type.func().name()});
                } else {
                    throw std::runtime_error("Unhandled explicit type parsing");
                }
                constraint_solver.AddConstraint(std::vector{var.symbol()}, [var, type](const constraint::Env& env) {
                    if (type.has_raw()) {
                        return env.At(var.symbol()).to_string() == type.raw().name();
                    }

                    return false;
                });
            } else {
                std::cout << "Malformed Explicit Constraint" << std::endl;
                return std::nullopt;
            }
        } else {
            std::cout << "Unknown Constraint Type" << std::endl;
            return std::nullopt;
        }
    }

    using DistanceType = constraint::Node::distance_type;
    const auto numVariables = (DistanceType)all_variable_names.size();
    auto heuristic = [heuristics = std::move(heuristcFuncs), numVariables](const constraint::StateQuery& state) {
        // Calculate the difference, allows us to measure meaningful progress
        DistanceType sum = numVariables + (DistanceType)state.NumConstraints() - (DistanceType)state.NumSatisfied();
        for (const auto& H : heuristics) {
            sum += H(state);
        }
        return sum;
    };

    auto actualDistance = [actual = std::move(distanceFuncs), numVariables](const constraint::StateQuery& state) {
        // Calculate the difference, allows us to measure meaningful progress
        DistanceType sum = numVariables + (DistanceType)state.NumConstraints();
        for (const auto& G : actual) {
            sum += G(state);
        }
        return sum;
    };

    const auto solution = constraint_solver.GetOptimizedSolution(std::move(heuristic), std::move(actualDistance));
    const auto hasSolution = solution.has_value();
    if (!hasSolution) {
        return std::nullopt;
    }

    ConstraintPass pass;
    for (const auto& var : all_variable_names) {
        if (!solution->Contains(var)) {
            // Not necessarily an error, as the caller could accept partial solutions.
            continue;
        }

        const auto val = solution->At(var);
        try {
            pass.SetResolvedType(var, TypeFromString(val.to_string(), *solution));
        } catch (...) {
            continue;
        }
    }
    return pass;
}
