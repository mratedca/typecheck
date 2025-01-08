#pragma once

#include "cpptest/cpptest.hpp"

#include "typecheck/GenericTypeGenerator.hpp"
#include "typecheck/TypeManager.hpp"

// Utility function to avoid boilerplate code in testing.
static inline void setupTypeManager(typecheck::TypeManager* tm) {
    CPPTEST_EXPECT_THAT(tm->registerType("int"));
    CPPTEST_EXPECT_THAT(tm->registerType("void"));
    CPPTEST_EXPECT_THAT(tm->registerType("float"));
    CPPTEST_EXPECT_THAT(tm->registerType("double"));
    CPPTEST_EXPECT_THAT(tm->setConvertible("int", "int"));
    CPPTEST_EXPECT_THAT(tm->setConvertible("int", "float"));
    CPPTEST_EXPECT_THAT(tm->setConvertible("int", "double"));
    CPPTEST_EXPECT_THAT(tm->setConvertible("double", "double"));
    CPPTEST_EXPECT_THAT(tm->setConvertible("float", "float"));
    CPPTEST_EXPECT_THAT(tm->setConvertible("float", "double"));
}

static inline auto CreateMultipleSymbols(typecheck::TypeManager& tm, const std::size_t& num) -> std::vector<typecheck::TypeVar> {
    std::vector<typecheck::TypeVar> out;
    for (std::size_t i = 0; i < num; ++i) {
        out.push_back(tm.CreateTypeVar());
    }
    return out;
}

#define getDefaultTypeManager(tm) \
    typecheck::TypeManager tm; \
    setupTypeManager(&tm)
