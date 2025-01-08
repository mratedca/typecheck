#include "cpptest/cpptest.hpp"
#include "typecheck/TypeVar.hpp"

class TypeVarTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(TypeVarTest)

NEW_TEST(TypeVarTest, CheckSetString) {
	typecheck::TypeVar t;
	t.set_symbol("Hello World");
	CPPTEST_EXPECT_EQ(t.symbol(), "Hello World");
}

NEW_TEST(TypeVarTest, CopyVar) {
	typecheck::TypeVar t;
	t.set_symbol("Hello World");
	typecheck::TypeVar g;
	g.CopyFrom(t);
	CPPTEST_EXPECT_EQ(g.symbol(), t.symbol());
	CPPTEST_EXPECT_EQ(g, t);
}

CPPTEST_END_CLASS(TypeVarTest)