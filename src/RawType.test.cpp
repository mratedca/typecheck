#include "cpptest/cpptest.hpp"
#include "typecheck/RawType.hpp"
#include "typecheck/Type.hpp"

class RawTypeTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(RawTypeTest)

NEW_TEST(RawTypeTest, CheckRawTypeCopyConstructor) {
	typecheck::RawType t;
	t.set_name("Hello World");
	CPPTEST_EXPECT_EQ(t.name(), "Hello World");
	typecheck::RawType g{t};
	CPPTEST_EXPECT_EQ(g.name(), "Hello World");
}

NEW_TEST(RawTypeTest, CheckRawTypeEquals) {
	typecheck::RawType t;
	t.set_name("Hello World");
	CPPTEST_EXPECT_EQ(t.name(), "Hello World");
	typecheck::RawType g{t};
	CPPTEST_EXPECT_EQ(g.name(), "Hello World");
	CPPTEST_EXPECT_EQ(g, t);
	g.set_name("Not Hello");
	CPPTEST_EXPECT_NEQ(g, t);
}

CPPTEST_END_CLASS(RawTypeTest)
