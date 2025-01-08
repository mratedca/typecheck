#pragma once

#include "../LiteralProtocol.hpp"

namespace typecheck {
	class ExpressibleByIntegerLiteral final : public LiteralProtocol {
	public:
		ExpressibleByIntegerLiteral() noexcept = default;
		~ExpressibleByIntegerLiteral() noexcept override = default;

		[[nodiscard]] auto getPreferredTypes() const noexcept -> std::vector<Type> override;
		[[nodiscard]] auto getOtherTypes() const noexcept -> std::vector<Type> override;
	};
}
