#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace typecheck {
	class Type;

	class FunctionDefinition {
	public:
		FunctionDefinition() noexcept;
		~FunctionDefinition() = default;

		FunctionDefinition(const FunctionDefinition& other);
		auto operator=(const FunctionDefinition& other) -> FunctionDefinition&;

		auto operator==(const FunctionDefinition& other) const noexcept -> bool;
		auto operator!=(const FunctionDefinition& other) const noexcept -> bool;

		void CopyFrom(const FunctionDefinition& other);

		auto args_size() const -> std::size_t;
		auto args(std::size_t i) const -> const Type&;
		auto add_args() -> Type*;

		auto returntype() const -> const Type&;
		auto mutable_returntype() -> Type*;
		auto has_returntype() const -> bool;

		auto name() const -> std::string;
        void set_name(const std::string& name);

		auto id() const -> long long;
        void set_id(long long id);

		auto ShortDebugString() const -> std::string;

	private:
		std::vector<Type> _args;

		// This one is a ptr so we can forward-declare it
		mutable std::unique_ptr<Type> _returnType;

		std::string _name;
		long long _id;
	};
}
