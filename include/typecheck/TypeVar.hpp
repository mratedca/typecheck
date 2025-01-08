#pragma once

#include <string>

namespace typecheck {
	class TypeVar {
	public:
		TypeVar(std::string s = "");
		~TypeVar() = default;

		auto operator==(const TypeVar& other) const noexcept -> bool;
		auto operator!=(const TypeVar& other) const noexcept -> bool;
		auto operator<(const TypeVar& other) const noexcept -> bool;

		void CopyFrom(const TypeVar& other);

		[[nodiscard]] auto symbol() const -> std::string;
		void set_symbol(const std::string& s);

		[[nodiscard]] auto ShortDebugString() const -> std::string;
	private:
		std::string _symbol;
	};
}
