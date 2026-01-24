#include "cpptest/cpptest.hpp"
#include "typecheck/GenericType.hpp"
#include "typecheck/Type.hpp"

class GenericTypeTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(GenericTypeTest)

// Test creation with 0 parameters (simple type)
NEW_TEST(GenericTypeTest, SimpleType) {
    typecheck::GenericType intType("int");
    CPPTEST_EXPECT_EQ(intType.name(), "int");
    CPPTEST_EXPECT_EQ(intType.type_params_size(), 0);
    CPPTEST_EXPECT_FALSE(intType.is_generic());
}

// Test creation with type parameters
NEW_TEST(GenericTypeTest, ArrayType) {
    typecheck::GenericType intType("int");
    typecheck::GenericType arrayType("Array");
    arrayType.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    CPPTEST_EXPECT_EQ(arrayType.name(), "Array");
    CPPTEST_EXPECT_EQ(arrayType.type_params_size(), 1);
    CPPTEST_EXPECT_THAT(arrayType.is_generic());
}

// Test nested generics
NEW_TEST(GenericTypeTest, NestedArrayType) {
    // Array<Array<int>>
    typecheck::GenericType intType("int");
    typecheck::GenericType innerArray("Array");
    innerArray.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    typecheck::GenericType outerArray("Array");
    outerArray.add_type_param()->CopyFrom(typecheck::Type(innerArray));
    
    CPPTEST_EXPECT_EQ(outerArray.type_params_size(), 1);
    CPPTEST_EXPECT_THAT(outerArray.type_params(0).has_generic());
    CPPTEST_EXPECT_EQ(outerArray.type_params(0).generic().name(), "Array");
}

// Test equality
NEW_TEST(GenericTypeTest, EqualitySimpleTypes) {
    typecheck::GenericType int1("int");
    typecheck::GenericType int2("int");
    
    CPPTEST_EXPECT_EQ(int1, int2);
}

NEW_TEST(GenericTypeTest, InequalityDifferentNames) {
    typecheck::GenericType intType("int");
    typecheck::GenericType stringType("string");
    
    CPPTEST_EXPECT_NEQ(intType, stringType);
}

NEW_TEST(GenericTypeTest, EqualityWithParams) {
    typecheck::GenericType intType("int");
    
    typecheck::GenericType array1("Array");
    array1.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    typecheck::GenericType array2("Array");
    array2.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    CPPTEST_EXPECT_EQ(array1, array2);
}

NEW_TEST(GenericTypeTest, InequalityDifferentParams) {
    typecheck::GenericType intType("int");
    typecheck::GenericType stringType("string");
    
    typecheck::GenericType arrayInt("Array");
    arrayInt.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    typecheck::GenericType arrayString("Array");
    arrayString.add_type_param()->CopyFrom(typecheck::Type(stringType));
    
    CPPTEST_EXPECT_NEQ(arrayInt, arrayString);
}

NEW_TEST(GenericTypeTest, InequalityParamCountMismatch) {
    typecheck::GenericType simpleArray("Array");  // 0 params
    typecheck::GenericType arrayInt("Array");
    arrayInt.add_type_param()->CopyFrom(typecheck::Type(typecheck::GenericType("int")));
    
    CPPTEST_EXPECT_NEQ(simpleArray, arrayInt);
}

// Test ShortDebugString
NEW_TEST(GenericTypeTest, ShortDebugStringSimple) {
    typecheck::GenericType intType("int");
    CPPTEST_EXPECT_EQ(intType.ShortDebugString(), "int");
}

NEW_TEST(GenericTypeTest, ShortDebugStringArray) {
    typecheck::GenericType intType("int");
    typecheck::GenericType arrayType("Array");
    arrayType.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    CPPTEST_EXPECT_EQ(arrayType.ShortDebugString(), "Array<int>");
}

NEW_TEST(GenericTypeTest, ShortDebugStringNestedArray) {
    typecheck::GenericType intType("int");
    typecheck::GenericType innerArray("Array");
    innerArray.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    typecheck::GenericType outerArray("Array");
    outerArray.add_type_param()->CopyFrom(typecheck::Type(innerArray));
    
    CPPTEST_EXPECT_EQ(outerArray.ShortDebugString(), "Array<Array<int>>");
}

// Test CopyFrom
NEW_TEST(GenericTypeTest, CopyFrom) {
    typecheck::GenericType intType("int");
    typecheck::GenericType arrayType("Array");
    arrayType.add_type_param()->CopyFrom(typecheck::Type(intType));
    
    typecheck::GenericType copiedType;
    copiedType.CopyFrom(arrayType);
    
    CPPTEST_EXPECT_EQ(copiedType.name(), "Array");
    CPPTEST_EXPECT_EQ(copiedType.type_params_size(), 1);
    CPPTEST_EXPECT_EQ(copiedType, arrayType);
}

CPPTEST_END_CLASS(GenericTypeTest)
