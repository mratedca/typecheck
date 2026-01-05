#include "cpptest/cpptest.hpp"
#include "typecheck/FunctionDefinition.hpp"
#include "typecheck/Type.hpp"

class FunctionDefinitionTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(FunctionDefinitionTest)

NEW_TEST(FunctionDefinitionTest, CheckAddArgs) {
	typecheck::FunctionDefinition f;
	CPPTEST_EXPECT_EQ(f.args_size(), 0);
	f.add_args();
	CPPTEST_EXPECT_EQ(f.args_size(), 1);
}

NEW_TEST(FunctionDefinitionTest, CheckCreateReturnType) {
	typecheck::FunctionDefinition f;
	CPPTEST_EXPECT_FALSE(f.has_returntype());
	f.returntype();
	CPPTEST_EXPECT_THAT(f.has_returntype());
}

NEW_TEST(FunctionDefinitionTest, CheckMutateReturnType) {
	typecheck::FunctionDefinition f;
	CPPTEST_EXPECT_FALSE(f.has_returntype());
	f.mutable_returntype()->mutable_generic()->set_name("Hello World");
	CPPTEST_EXPECT_THAT(f.returntype().has_generic());
	CPPTEST_EXPECT_EQ(f.returntype().generic().name(), "Hello World");
}

NEW_TEST(FunctionDefinitionTest, CheckCopyFrom) {
	typecheck::FunctionDefinition f;

	CPPTEST_EXPECT_EQ(f.args_size(), 0);
	f.add_args();
	CPPTEST_EXPECT_EQ(f.args_size(), 1);

	CPPTEST_EXPECT_FALSE(f.has_returntype());
	f.mutable_returntype()->mutable_generic()->set_name("Hello World");
	CPPTEST_EXPECT_THAT(f.returntype().has_generic());
	CPPTEST_EXPECT_EQ(f.returntype().generic().name(), "Hello World");

	typecheck::FunctionDefinition g;
	g.CopyFrom(f);
	CPPTEST_EXPECT_EQ(g.args_size(), 1);
	CPPTEST_EXPECT_THAT(g.has_returntype());
	CPPTEST_EXPECT_THAT(g.returntype().has_generic());
	CPPTEST_EXPECT_EQ(g.returntype().generic().name(), "Hello World");
}

NEW_TEST(FunctionDefinitionTest, CheckEquals) {
	typecheck::FunctionDefinition f;
	f.set_id(123);
	f.add_args();
	f.mutable_returntype()->mutable_generic()->set_name("Hello World");

	typecheck::FunctionDefinition g;
	g.set_id(123);
	g.add_args();
	g.mutable_returntype()->mutable_generic()->set_name("Hello World");
	CPPTEST_EXPECT_EQ(f, g);

	g.add_args();
	CPPTEST_EXPECT_NEQ(f, g);
}

CPPTEST_END_CLASS(FunctionDefinitionTest)
