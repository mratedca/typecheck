#pragma once

#include "Type.hpp"
#include "KnownProtocolKind.hpp"
#include "TypeVar.hpp"

#include <optional>
#include <cstdint>

namespace typecheck {
	enum ConstraintKind {
		Bind = 0,
		Equal,
		BindParam,
		Conversion,
		ConformsTo,
		ApplicableFunction,
		BindOverload,
	};

	enum ConstraintRestrictionKind {
		DeepEquality = 0,
		Superclass,
		ArrayToPointer,
		StringToPointer,
		ValueToOptional,
	};


	// Taken from https://github.com/apple/swift/lib/Sema/Constraint.h
	enum ConstraintClassification {
		/// A relational constraint, which relates two types.
		Relational = 0,

		/// A member constraint, which names a member of a type and assigns it a reference type.
		Member,

		/// A property of a single type, such as whether it is defaultable to a particular type.
		TypeProperty,

		 /// A disjunction constraint.
		Disjunction,
	};

	class Constraint {
	public:
        using IDType = long long;

		class Types {
		public:
			Types();
			Types(const Types& other);
			auto operator=(const Types& other) -> Types&;
			Types(Types&& other) noexcept;
			auto operator=(Types&& other) noexcept -> Types&;

			auto has_first() const -> bool;
			auto first() const -> const TypeVar&;
			auto mutable_first() -> TypeVar*;

			auto has_second() const -> bool;
			auto second() const -> const TypeVar&;
			auto mutable_second() -> TypeVar*;

			auto has_third() const -> bool;
			auto third() const -> const TypeVar&;
			auto mutable_third() -> TypeVar*;

			auto ShortDebugString() const -> std::string;

		private:
			mutable std::optional<TypeVar> _first;
			mutable std::optional<TypeVar> _second;
			mutable std::optional<TypeVar> _third;
		};

		class ExplicitType {
		public:
			ExplicitType();
			ExplicitType(const ExplicitType& other);
			ExplicitType(ExplicitType&& other) noexcept;
			auto operator=(ExplicitType&& other) noexcept -> ExplicitType&;

			auto has_type() const -> bool;
			auto mutable_type() -> Type*;
			auto type() const -> const Type&;

			auto has_var() const -> bool;
			auto mutable_var() -> TypeVar*;
			auto var() const -> const TypeVar&;

			auto ShortDebugString() const -> std::string;

		private:
			mutable std::optional<TypeVar> _var;
			mutable std::optional<Type> _type;
		};

		class Member {
		public:
			[[nodiscard]] auto first() const -> const TypeVar&;
			[[nodiscard]] auto second() const -> const TypeVar&;

			[[nodiscard]] auto ShortDebugString() const -> std::string;

		private:
			TypeVar _first;
			TypeVar _second;

			// Pointer to the declared context
			// std::int64_t declContextPtr;
		};

		class Overload {
		public:
			Overload();
			Overload(const Overload& other);
			auto operator=(const Overload& other) -> Overload&;
			Overload(Overload&& other) noexcept;
			auto operator=(Overload&& other) noexcept -> Overload&;

			auto functionid() const -> long long;
			void set_functionid(long long functionid);

			auto argvars_size() const -> std::size_t;
			auto argvars(std::size_t i) const -> const TypeVar&;
			auto add_argvars() -> TypeVar*;

			auto has_type() const -> bool;
			auto type() const -> const TypeVar&;
			auto mutable_type() -> TypeVar*;

			auto returnvar() const -> const TypeVar&;
			auto mutable_returnvar() -> TypeVar*;

			auto ShortDebugString() const -> std::string;

		private:
			mutable std::optional<TypeVar> _type;
			long long _functionID{};
			std::vector<TypeVar> _argVars;
			TypeVar _returnVar;

			// Pointer to the declared context
			//std::int64_t declContextPtr;
		};

		class Conforms {
		public:
			Conforms();
			Conforms(const Conforms& other);
			auto operator=(const Conforms& other) -> Conforms&;
			Conforms(Conforms&& other) noexcept;
			auto operator=(Conforms&& other) noexcept -> Conforms&;

			auto has_protocol() const -> bool;
			auto protocol() const -> const KnownProtocolKind&;
			auto mutable_protocol() -> KnownProtocolKind*;

			auto has_type() const -> bool;
			auto type() const -> const TypeVar&;
			auto mutable_type() -> TypeVar*;
			auto ShortDebugString() const -> std::string;
		private:
			mutable std::optional<KnownProtocolKind> _protocol;
			mutable std::optional<TypeVar> _type;
		};

		Constraint();
		auto operator==(const Constraint& other) const -> bool;

		auto id() const -> IDType;
		void set_id(IDType id);

		auto kind() const -> const ConstraintKind&;
		void set_kind(const ConstraintKind& kind);

		auto has_explicit() const -> bool;
		auto mutable_explicit() -> ExplicitType*;
		auto explicit_() const -> const ExplicitType&;

		auto has_overload() const -> bool;
		auto mutable_overload() -> Overload*;
		auto overload() const -> const Overload&;

		auto has_conforms() const -> bool;
		auto mutable_conforms() -> Conforms*;
		auto conforms() const -> const Conforms&;

		auto has_types() const -> bool;
		auto mutable_types() -> Types*;
		auto types() const -> const Types&;

		auto ShortDebugString() const -> std::string;
	private:
		ConstraintKind _kind;
		/*
		ConstraintRestrictionKind restriction;

		bool hasRestriction;
		bool isActive;
		bool isDisabled;
		bool isFavoured;
		*/
		long long _id;

		// ID's of nested constraints
		std::vector<std::int64_t> nested;

		mutable std::variant<bool, Types, Member, Overload, Conforms, ExplicitType> data;
	};
}
