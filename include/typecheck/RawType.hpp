#pragma once

#include <string>

namespace typecheck {
	class RawType {
	public:
		RawType(std::string  n = "");
		~RawType() = default;

		void CopyFrom(const RawType& other);

		auto operator==(const RawType& other) const noexcept -> bool;
		auto operator!=(const RawType& other) const noexcept -> bool;

		[[nodiscard]] auto name() const -> std::string;
		void set_name(const std::string& name);

		[[nodiscard]] auto ShortDebugString() const -> std::string;
	private:
		std::string _name;
	};
}
