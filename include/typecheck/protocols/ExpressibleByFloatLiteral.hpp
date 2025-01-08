#pragma once

#include "../LiteralProtocol.hpp"

namespace typecheck {
	class ExpressibleByFloatLiteral final : public LiteralProtocol {
	public:
		ExpressibleByFloatLiteral() noexcept = default;
		~ExpressibleByFloatLiteral() noexcept override = default;

		[[nodiscard]] auto getPreferredTypes() const noexcept -> std::vector<Type> override;
		[[nodiscard]] auto getOtherTypes() const noexcept -> std::vector<Type> override;
	};
}
