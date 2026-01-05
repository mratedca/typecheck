#pragma once

#include "Type.hpp"

#include <vector>

namespace typecheck {
	class LiteralProtocol {
	protected:
		[[nodiscard]] auto ty(const std::string& _ty) const noexcept -> Type {
			Type ty;
            ty.mutable_generic()->set_name(_ty);
			return ty;
		}

	public:
		LiteralProtocol() noexcept = default;
		virtual ~LiteralProtocol() noexcept = default;

        // LCOV_EXCL_START
		virtual auto getPreferredTypes() const noexcept -> std::vector<Type> { return {}; }
		virtual auto getOtherTypes() const noexcept -> std::vector<Type> { return {}; }
        // LCOV_EXCL_STOP
	};
}
