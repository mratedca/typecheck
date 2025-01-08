#include "typecheck/GenericTypeGenerator.hpp"

auto typecheck::GenericTypeGenerator::next_id() -> long long {
	return this->curr_num++;
}

auto typecheck::GenericTypeGenerator::next() -> std::string {
    return "T" + std::to_string(this->next_id());
}
