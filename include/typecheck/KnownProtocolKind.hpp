#pragma once

#include <string>
#include <variant>

namespace typecheck {
	class KnownProtocolKind {
	public:
		enum DefaultProtocol {
			Sequence = 0,
			IteratorProtocol,
			RawRepresentable,
			Equatable,
			Hashable,
			Comparable,
			Error,
			OptionSet,
			CaseIterable,

			CodingKey,
			Encodable,
			Decodable,

			StringInterpolation,
		};

		enum LiteralProtocol {
			// Start of Literal Protocols
			ExpressibleByArray = 0,
			ExpressibleByBoolean,
			ExpressibleByDictionary,
			ExpressibleByFloat,
			ExpressibleByInteger,
            ExpressibleByDouble,
			// ExpressibleByStringInterpolation,
			ExpressibleByString,
			ExpressibleByNil,
			// ExpressibleByUnicodeScalar,

			// ExpressibleByColour,
			// ExpressibleByImage,
			// ExpressibleByFile,
		};

		KnownProtocolKind() : _data(false) {}
		KnownProtocolKind(const DefaultProtocol& d) : _data(d) {}
		KnownProtocolKind(const LiteralProtocol& l) : _data(l) {}
		~KnownProtocolKind() = default;

		[[nodiscard]] auto literal() const -> const LiteralProtocol&;
		void set_literal(const LiteralProtocol& literal);
		[[nodiscard]] auto has_literal() const -> bool;

		[[nodiscard]] auto ShortDebugString() const -> std::string;
	private:
		std::variant<DefaultProtocol, LiteralProtocol, bool> _data;
	};
}
