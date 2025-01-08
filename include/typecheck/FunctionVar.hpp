#pragma once

#include "TypeVar.hpp"

#include <string>
#include <vector>

namespace typecheck {
	class FunctionVar {
	public:
		FunctionVar();
		~FunctionVar() = default;

		[[nodiscard]] auto id() const -> long long;
		void set_id(long long id);

		auto mutable_returnvar() -> TypeVar*;
		[[nodiscard]] auto returnvar() const -> const TypeVar&;

		[[nodiscard]] auto args() const -> const std::vector<TypeVar>&;
		auto add_args() -> TypeVar*;

        [[nodiscard]] auto serialize() const -> std::string;
        static auto unserialize(const std::string& str) -> FunctionVar;

        [[nodiscard]] auto name() const -> std::string;

	private:
		std::vector<TypeVar> _args;
		TypeVar _returnVar;
		std::string _name;
		long long _id;
	};
}
