#include "cpptest/cpptest.hpp"
#include "typecheck/Type.hpp"

class TypeTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(TypeTest)

NEW_TEST(TypeTest, CheckHasFunc) {
	typecheck::Type t;
	CPPTEST_EXPECT_FALSE(t.has_func());

	// Should create func
	t.func();
	CPPTEST_EXPECT_THAT(t.has_func());
}

NEW_TEST(TypeTest, CheckHasRaw) {
	typecheck::Type t;
	CPPTEST_EXPECT_FALSE(t.has_raw());

	t.raw();
	CPPTEST_EXPECT_THAT(t.has_raw());
}

NEW_TEST(TypeTest, CheckHasMutableFunc) {
	typecheck::Type t;
	CPPTEST_EXPECT_FALSE(t.has_func());

	// Should create func
	t.mutable_func();
	CPPTEST_EXPECT_THAT(t.has_func());
}

NEW_TEST(TypeTest, CheckHasMutableRaw) {
	typecheck::Type t;
	CPPTEST_EXPECT_FALSE(t.has_raw());

	t.mutable_raw();
	CPPTEST_EXPECT_THAT(t.has_raw());
}

NEW_TEST(TypeTest, MutableRawHolds) {
	typecheck::Type t;
	t.mutable_raw()->set_name("Hello world");
	CPPTEST_EXPECT_EQ(t.raw().name(), "Hello world");
}

NEW_TEST(TypeTest, MutableFuncHolds) {
	typecheck::Type t;
	t.mutable_func()->mutable_returntype()->mutable_raw()->set_name("Hello world");
	CPPTEST_EXPECT_THAT(t.has_func());
	CPPTEST_EXPECT_THAT(t.func().has_returntype());
	CPPTEST_EXPECT_THAT(t.func().returntype().has_raw());
	CPPTEST_EXPECT_EQ(t.func().returntype().raw().name(), "Hello world");
}

NEW_TEST(TypeTest, CopyTypeConstructor) {
	typecheck::Type t;
	t.mutable_func()->mutable_returntype()->mutable_raw()->set_name("Hello world");
	CPPTEST_EXPECT_THAT(t.has_func());
	CPPTEST_EXPECT_THAT(t.func().has_returntype());
	CPPTEST_EXPECT_THAT(t.func().returntype().has_raw());
	CPPTEST_EXPECT_EQ(t.func().returntype().raw().name(), "Hello world");

	typecheck::Type g{t};
	CPPTEST_EXPECT_THAT(g.has_func());
	CPPTEST_EXPECT_THAT(g.func().has_returntype());
	CPPTEST_EXPECT_THAT(g.func().returntype().has_raw());
	CPPTEST_EXPECT_EQ(g.func().returntype().raw().name(), "Hello world");
}

CPPTEST_END_CLASS(TypeTest)
