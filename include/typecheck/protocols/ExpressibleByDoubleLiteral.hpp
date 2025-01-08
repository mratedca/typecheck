#pragma once

#include "../LiteralProtocol.hpp"

namespace typecheck {
	class ExpressibleByDoubleLiteral final : public LiteralProtocol {
	public:
		ExpressibleByDoubleLiteral() noexcept = default;
		~ExpressibleByDoubleLiteral() noexcept override = default;

		[[nodiscard]] auto getPreferredTypes() const noexcept -> std::vector<Type> override;
		[[nodiscard]] auto getOtherTypes() const noexcept -> std::vector<Type> override;
	};
}
