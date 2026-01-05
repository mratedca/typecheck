#include "cpptest/cpptest.hpp"
#include "Utils.test.hpp"

class ArrayFunctionReturnTest : public cpptest::BaseCppTest {
public:
    void SetUp() {}
    void TearDown() {}
};

CPPTEST_CLASS(ArrayFunctionReturnTest)

// Test: function foo() -> [String] { return []; }
NEW_TEST(ArrayFunctionReturnTest, EmptyArrayWithExplicitReturnType) {
    getDefaultTypeManager(tm);
    tm.registerType("string");
    
    // Function return type annotation: [String]
    auto returnTypeVar = tm.CreateTypeVar();
    auto returnElementVar = tm.CreateTypeVar();
    tm.CreateArrayElementConstraint(returnTypeVar, returnElementVar);
    tm.CreateBindToConstraint(returnElementVar, tm.getRegisteredType("string"));
    
    // Empty array in return statement: []
    auto emptyArrayVar = tm.CreateTypeVar();
    auto emptyElementVar = tm.CreateTypeVar();
    tm.CreateArrayElementConstraint(emptyArrayVar, emptyElementVar);
    
    // Return statement: empty array equals return type
    tm.CreateEqualsConstraint(emptyArrayVar, returnTypeVar);
    
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    
    auto resolvedEmpty = solution->GetResolvedType(emptyArrayVar);
    CPPTEST_EXPECT_TRUE(resolvedEmpty.has_generic());
    CPPTEST_EXPECT_EQ(resolvedEmpty.generic().name(), "Array");
    CPPTEST_EXPECT_EQ(resolvedEmpty.generic().type_params_size(), 1);
    CPPTEST_EXPECT_TRUE(resolvedEmpty.generic().type_params(0).has_generic());
    CPPTEST_EXPECT_EQ(resolvedEmpty.generic().type_params(0).generic().name(), "string");
}

CPPTEST_END_CLASS(ArrayFunctionReturnTest)

