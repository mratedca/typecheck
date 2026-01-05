#pragma once

#include <string>
#include <vector>

namespace typecheck {
	// Forward declaration
	class Type;

	class GenericType {
	public:
		GenericType(std::string n = "");
		~GenericType() = default;

		void CopyFrom(const GenericType& other);

		auto operator==(const GenericType& other) const noexcept -> bool;
		auto operator!=(const GenericType& other) const noexcept -> bool;

		[[nodiscard]] auto name() const -> std::string;
		void set_name(const std::string& name);

		[[nodiscard]] auto type_params_size() const -> std::size_t;
		[[nodiscard]] auto type_params(std::size_t i) const -> const Type&;
		auto mutable_type_params(std::size_t i) -> Type*;
		auto add_type_param() -> Type*;

		[[nodiscard]] auto is_generic() const -> bool;

		[[nodiscard]] auto ShortDebugString() const -> std::string;

	private:
		std::string _name;
		std::vector<Type> _typeParams;
	};
}

