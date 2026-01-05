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
	CPPTEST_EXPECT_FALSE(t.has_generic());

	t.generic();
	CPPTEST_EXPECT_THAT(t.has_generic());
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
	CPPTEST_EXPECT_FALSE(t.has_generic());

	t.mutable_generic();
	CPPTEST_EXPECT_THAT(t.has_generic());
}

NEW_TEST(TypeTest, MutableRawHolds) {
	typecheck::Type t;
	t.mutable_generic()->set_name("Hello world");
	CPPTEST_EXPECT_EQ(t.generic().name(), "Hello world");
}

NEW_TEST(TypeTest, MutableFuncHolds) {
	typecheck::Type t;
	t.mutable_func()->mutable_returntype()->mutable_generic()->set_name("Hello world");
	CPPTEST_EXPECT_THAT(t.has_func());
	CPPTEST_EXPECT_THAT(t.func().has_returntype());
	CPPTEST_EXPECT_THAT(t.func().returntype().has_generic());
	CPPTEST_EXPECT_EQ(t.func().returntype().generic().name(), "Hello world");
}

NEW_TEST(TypeTest, CopyTypeConstructor) {
	typecheck::Type t;
	t.mutable_func()->mutable_returntype()->mutable_generic()->set_name("Hello world");
	CPPTEST_EXPECT_THAT(t.has_func());
	CPPTEST_EXPECT_THAT(t.func().has_returntype());
	CPPTEST_EXPECT_THAT(t.func().returntype().has_generic());
	CPPTEST_EXPECT_EQ(t.func().returntype().generic().name(), "Hello world");

	typecheck::Type g{t};
	CPPTEST_EXPECT_THAT(g.has_func());
	CPPTEST_EXPECT_THAT(g.func().has_returntype());
	CPPTEST_EXPECT_THAT(g.func().returntype().has_generic());
	CPPTEST_EXPECT_EQ(g.func().returntype().generic().name(), "Hello world");
}

// Test Type equality with GenericType
NEW_TEST(TypeTest, SimpleTypeEquality) {
	typecheck::GenericType int1("int");
	typecheck::GenericType int2("int");
	
	typecheck::Type t1(int1);
	typecheck::Type t2(int2);
	
	CPPTEST_EXPECT_EQ(t1, t2);
}

NEW_TEST(TypeTest, GenericTypeWithParamsNotEqualToSimple) {
	typecheck::GenericType simpleArray("Array");  // 0 params
	typecheck::GenericType arrayInt("Array");
	arrayInt.add_type_param()->CopyFrom(typecheck::Type(typecheck::GenericType("int")));
	
	typecheck::Type t1(simpleArray);
	typecheck::Type t2(arrayInt);
	
	CPPTEST_EXPECT_NEQ(t1, t2);
}

NEW_TEST(TypeTest, GenericTypeWithSameParamsEqual) {
	typecheck::GenericType array1("Array");
	array1.add_type_param()->CopyFrom(typecheck::Type(typecheck::GenericType("int")));
	
	typecheck::GenericType array2("Array");
	array2.add_type_param()->CopyFrom(typecheck::Type(typecheck::GenericType("int")));
	
	typecheck::Type t1(array1);
	typecheck::Type t2(array2);
	
	CPPTEST_EXPECT_EQ(t1, t2);
}

CPPTEST_END_CLASS(TypeTest)
