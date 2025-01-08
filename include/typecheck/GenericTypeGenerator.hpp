#pragma once

#include <string>

namespace typecheck {
	class GenericTypeGenerator {
	public:
        using value_type = long long;
		GenericTypeGenerator() = default;
		~GenericTypeGenerator() = default;

        auto next() -> std::string;
        auto next_id() -> value_type;

    private:
        value_type curr_num = 0;
	};
}
