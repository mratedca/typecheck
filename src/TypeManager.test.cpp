#include "cpptest/cpptest.hpp"
#include "Utils.test.hpp"
#include "cpptest/internal/googletest.hpp"

class TypeManagerTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(TypeManagerTest)

NEW_TEST(TypeManagerTest, CreateFunctionhasNoArgs) {
    typecheck::TypeManager tm;
    CPPTEST_EXPECT_NEQ(tm.CreateFunctionHash("foo", {}),  0);
}

NEW_TEST(TypeManagerTest, CreateLambdaHashNoArgs) {
    typecheck::TypeManager tm;
    CPPTEST_EXPECT_NEQ(tm.CreateLambdaFunctionHash({}), 0);
}

NEW_TEST(TypeManagerTest, CreateFunctionHas1Args) {
    typecheck::TypeManager tm;
    CPPTEST_EXPECT_NEQ(tm.CreateFunctionHash("foo", {"a"}), 0);
}

NEW_TEST(TypeManagerTest, CreateLambdaHash1Arg) {
    typecheck::TypeManager tm;
    CPPTEST_EXPECT_NEQ(tm.CreateLambdaFunctionHash({"a"}), 0);
}

NEW_TEST(TypeManagerTest, CreateFunctionHash2Args) {
    typecheck::TypeManager tm;
    CPPTEST_EXPECT_NEQ(tm.CreateFunctionHash("foo", {"a", "b"}), 0);
}

NEW_TEST(TypeManagerTest, CreateLambdaHash2Args) {
    typecheck::TypeManager tm;
    CPPTEST_EXPECT_NEQ(tm.CreateLambdaFunctionHash({"a", "b"}) , 0);
}

NEW_TEST(TypeManagerTest, LoadBasicTypes) {
    typecheck::TypeManager tm;
    CPPTEST_ASSERT_THAT(tm.registerType("int"));
    CPPTEST_ASSERT_THAT(tm.registerType("float"));
    CPPTEST_ASSERT_THAT(tm.registerType("double"));
    CPPTEST_ASSERT_FALSE(tm.registerType("double"));
    CPPTEST_EXPECT_THAT(tm.hasRegisteredType("int"));
    CPPTEST_EXPECT_THAT(tm.hasRegisteredType("float"));
    CPPTEST_EXPECT_THAT(tm.hasRegisteredType("double"));
    CPPTEST_EXPECT_FALSE(tm.hasRegisteredType("string"));
}

NEW_TEST(TypeManagerTest, LoadBasicTypeConversions) {
    typecheck::TypeManager tm;
    CPPTEST_ASSERT_THAT(tm.registerType("int"));
    CPPTEST_ASSERT_THAT(tm.registerType("float"));
    CPPTEST_ASSERT_THAT(tm.registerType("double"));
    CPPTEST_ASSERT_THAT(tm.setConvertible("int", "double"));
    CPPTEST_ASSERT_THAT(tm.setConvertible("int", "float"));
    CPPTEST_ASSERT_THAT(tm.setConvertible("float", "double"));

    CPPTEST_EXPECT_THAT(tm.isConvertible("int", "int"));
    CPPTEST_EXPECT_THAT(tm.isConvertible("float", "float"));
    CPPTEST_EXPECT_THAT(tm.isConvertible("double", "double"));

    CPPTEST_EXPECT_THAT(tm.isConvertible("int", "double"));
    CPPTEST_EXPECT_THAT(tm.isConvertible("int", "float"));
    CPPTEST_EXPECT_THAT(tm.isConvertible("float", "double"));

    CPPTEST_EXPECT_FALSE(tm.isConvertible("double", "int"));
    CPPTEST_EXPECT_FALSE(tm.isConvertible("float", "int"));
    CPPTEST_EXPECT_FALSE(tm.isConvertible("double", "float"));
}

CPPTEST_END_CLASS(TypeManagerTest)