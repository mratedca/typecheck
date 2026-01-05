#include "cpptest/cpptest.hpp"
#include "Utils.test.hpp"

class ConstraintTest : public cpptest::BaseCppTest {
public:
    void SetUp() {
        // Run before every test
    }

    void TearDown() {
        // Run After every test
    }
};

CPPTEST_CLASS(ConstraintTest)

NEW_TEST(ConstraintTest, ResolveBindToConflictingFull) {
    getDefaultTypeManager(tm);
    auto T1 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(T1, tm.getRegisteredType("float"));

    CPPTEST_ASSERT_FALSE(tm.solve());
}

NEW_TEST(ConstraintTest, SolveBasicTypeIntEqualsConstraint) {
    getDefaultTypeManager(tm);

    // type(1), type(2);
    auto T1 = tm.CreateTypeVar(); // These are names of type variables, not actual types.
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T1, T2);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());

    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T1).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T2).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T1).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T2).generic().name(), "int");
}

NEW_TEST(ConstraintTest, SolveBasicTypeFloatEqualsConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateEqualsConstraint(T1, T2);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());

    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T1).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T2).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T1).generic().name(), "float");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T2).generic().name(), "float");
}

NEW_TEST(ConstraintTest, SolveBasicTypeEqualsMutuallyRecursiveConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateEqualsConstraint(T2, T1);
    tm.CreateEqualsConstraint(T1, T2);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveBasicTypeEqualsTriangleConstraint) {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

    tm.CreateEqualsConstraint(T.at(2) , T.at(0));
    tm.CreateEqualsConstraint(T.at(0), T.at(1));
    tm.CreateEqualsConstraint(T.at(1), T.at(2));

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveBasicTypeConformsToTriangleSolvableConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T1, T2);
    tm.CreateEqualsConstraint(T2, T3);
    tm.CreateEqualsConstraint(T3, T1);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveBasicTypeConformsToTriangleConversionSolvableConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByFloat);
    tm.CreateEqualsConstraint(T1, T2);
    tm.CreateEqualsConstraint(T2, T3);
    tm.CreateEqualsConstraint(T3, T1);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveConvertibleConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::LiteralProtocol::ExpressibleByInteger);
    tm.CreateConvertibleConstraint(T1, T2);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveConvertibleReverseConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateConvertibleConstraint(T2, T1);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveConvertibleBindReverseConstrant) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateConvertibleConstraint(T1, T2);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, SolveConvertibleConversionExplicitConstraint) {
    getDefaultTypeManager(tm);

    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();

    tm.CreateLiteralConformsToConstraint(T1, typecheck::KnownProtocolKind::ExpressibleByFloat);
    tm.CreateBindToConstraint(T2, tm.getRegisteredType("float"));
    tm.CreateEqualsConstraint(T2, T1);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T1).generic().name(), "float");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T2).generic().name(), "float");
}

NEW_TEST(ConstraintTest, SolveFunctionApplicationConstraint) {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 8);

    const auto T0FuncHash = std::hash<std::string>()(T.at(0).symbol());
    tm.CreateApplicableFunctionConstraint(T0FuncHash, {tm.getRegisteredType("int"), tm.getRegisteredType("float")}, tm.getRegisteredType("double"));
    tm.CreateBindFunctionConstraint(T0FuncHash, T.at(0), {T.at(1), T.at(2)}, T.at(3));

    // T0 = (T1, T2) -> T3
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());

    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).has_func());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(1)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(2)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(3)).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(1)).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(2)).generic().name(), "float");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(3)).generic().name(), "double");
}

NEW_TEST(ConstraintTest, SolveInferredFunctionApplicationConstraint) {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 7);

    const auto T0FuncHash = std::hash<std::string>()(T.at(0).symbol());
    tm.CreateApplicableFunctionConstraint(T0FuncHash, {T.at(1), T.at(2)}, T.at(3));
    tm.CreateBindFunctionConstraint(T0FuncHash, T.at(0), {T.at(4), T.at(5)}, T.at(6));

    // 'internal' constraints to the function
    tm.CreateBindToConstraint(T.at(1), tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(T.at(2), tm.getRegisteredType("float"));
    tm.CreateBindToConstraint(T.at(3), tm.getRegisteredType("double"));

    // T0 = (T1, T2) -> T3
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());

    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).has_func());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(1)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(2)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(3)).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(1)).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(2)).generic().name(), "float");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(3)).generic().name(), "double");
}

NEW_TEST(ConstraintTest, SolveInferredFunctionApplicationConstraintNoArgs) {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

    const auto T0FuncHash = std::hash<std::string>()(T.at(0).symbol());
    tm.CreateApplicableFunctionConstraint(T0FuncHash, {}, T.at(1));
    tm.CreateBindFunctionConstraint(T0FuncHash, T.at(0), {}, T.at(2));

    // 'internal' constraints to the function
    tm.CreateBindToConstraint(T.at(1), tm.getRegisteredType("double"));

    // T0 = (T1, T2) -> T3
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());

    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).has_func());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(1)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(2)).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(1)).generic().name(), "double");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(2)).generic().name(), "double");
}

NEW_TEST(ConstraintTest, SolveFunctionDifferentNumArgsApplicationConstraint) {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

    // func foo(a: Int, b: Float) -> Double
    const auto T0FuncHash = std::hash<std::string>()("foo:a:b");
    tm.CreateApplicableFunctionConstraint(T0FuncHash, { tm.getRegisteredType("int"), tm.getRegisteredType("float") }, tm.getRegisteredType("double"));

    // func foo(a: Int) -> Double
    const auto T1FuncHash = std::hash<std::string>()("foo:a");
    tm.CreateApplicableFunctionConstraint(T1FuncHash, { tm.getRegisteredType("int") }, tm.getRegisteredType("double"));

    tm.CreateBindFunctionConstraint(T1FuncHash, T.at(0), { T.at(1) }, T.at(2));

    // T0 = (T1, T2) -> T3
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).has_func());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(1)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(2)).has_generic());

    // Check it got bound to: func foo(a: Int) -> Double
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).func().args_size(), 1);
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).func().args(0).has_generic());
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).func().args(0).generic().name(), "int");
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).func().has_returntype());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).func().returntype().has_generic());
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).func().returntype().generic().name(), "double");

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(1)).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(2)).generic().name(), "double");
}

NEW_TEST(ConstraintTest, SolveFunctionSameNumArgsDifferentTypeApplicationConstraint) {
    getDefaultTypeManager(tm);

    const auto T = CreateMultipleSymbols(tm, 3);

    // func foo(a: Int) -> Double
    // (T3) -> T4
    tm.CreateApplicableFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), { tm.getRegisteredType("int") }, tm.getRegisteredType("double"));

    // func foo(a: Float) -> Double
    // (T5) -> T6
    tm.CreateApplicableFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), { tm.getRegisteredType("float") }, tm.getRegisteredType("double"));

    tm.CreateBindFunctionConstraint(tm.CreateFunctionHash("foo", {"a"}), T.at(0), { T.at(1) }, T.at(2));
    tm.CreateLiteralConformsToConstraint(T.at(1), typecheck::KnownProtocolKind::ExpressibleByInteger);

    // T0 = (T1, T2) -> T3
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).has_func());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(1)).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(2)).has_generic());

    // Check it got bound to: func foo(a: Int) -> Double
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).func().args_size(), 1);
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).func().args(0).has_generic());
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).func().args(0).generic().name(), "int");
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).func().has_returntype());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(0)).func().returntype().has_generic());
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).func().returntype().generic().name(), "double");

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(1)).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(2)).generic().name(), "double");
}

NEW_TEST(ConstraintTest, SolveFunctionInferArgsLaterConstraint) {
    getDefaultTypeManager(tm);
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 9);

    const auto functionID = tm.CreateFunctionHash("foo", {});
    tm.CreateApplicableFunctionConstraint(functionID, {}, T.at(0));

    tm.CreateBindToConstraint(T.at(2), tm.getRegisteredType("void"));
    tm.CreateBindToConstraint(T.at(3), tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T.at(4), T.at(3));
    tm.CreateEqualsConstraint(T.at(5), T.at(3));
    tm.CreateEqualsConstraint(T.at(6), T.at(5));
    tm.CreateEqualsConstraint(T.at(0), T.at(6));
    tm.CreateBindFunctionConstraint(functionID, T.at(8), {}, T.at(7));

    /*
     T8 = () -> T7
     T8() -> T7 {
        ...
        T6
     }
     */
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(8)).has_func());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(7)).has_generic());

    // Check it got bound to: func foo() -> Int
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(8)).func().has_returntype());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(8)).func().returntype().has_generic());
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(8)).func().returntype().generic().name(), "int");
}

NEW_TEST(ConstraintTest, SolveForLoopConstraintsRegression) {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    auto T0 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();
    auto T4 = tm.CreateTypeVar();
    auto T5 = tm.CreateTypeVar();
    auto T7 = tm.CreateTypeVar();
    auto T9 = tm.CreateTypeVar();

    tm.CreateEqualsConstraint(T0, T3);
    tm.CreateEqualsConstraint(T3, T4);
	tm.CreateLiteralConformsToConstraint(T4, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T5, T3);
    tm.CreateEqualsConstraint(T7, T0);
    tm.CreateEqualsConstraint(T9, T7);

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T0).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T3).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T4).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T5).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T7).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T9).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T0).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T3).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T4).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T5).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T7).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T9).generic().name(), "int");
}

NEW_TEST(ConstraintTest, SolveForLoopConstraints) {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    auto T0 = tm.CreateTypeVar();
    auto T1 = tm.CreateTypeVar();
    auto T2 = tm.CreateTypeVar();
    auto T3 = tm.CreateTypeVar();
    auto T4 = tm.CreateTypeVar();
    auto T5 = tm.CreateTypeVar();
    auto T6 = tm.CreateTypeVar();
    auto T7 = tm.CreateTypeVar();
    auto T8 = tm.CreateTypeVar();
    auto T9 = tm.CreateTypeVar();
    auto T10 = tm.CreateTypeVar();
    auto T11 = tm.CreateTypeVar();
    auto T12 = tm.CreateTypeVar();

    tm.CreateEqualsConstraint(T0, T3);
    tm.CreateBindToConstraint(T1, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T2, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T3, T4);
    tm.CreateLiteralConformsToConstraint(T4, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T5, T3);
    tm.CreateBindToConstraint(T6, tm.getRegisteredType("bool"));
    tm.CreateEqualsConstraint(T7, T0);
    tm.CreateLiteralConformsToConstraint(T8, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T9, T7);
    tm.CreateBindToConstraint(T10, tm.getRegisteredType("int"));
    tm.CreateLiteralConformsToConstraint(T11, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateBindToConstraint(T12, tm.getRegisteredType("void"));

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T0).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T1).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T2).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T3).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T4).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T5).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T6).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T7).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T8).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T9).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T10).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T11).has_generic());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T12).has_generic());

    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T0).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T1).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T2).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T3).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T4).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T5).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T6).generic().name(), "bool");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T7).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T8).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T9).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T10).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T11).generic().name(), "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T12).generic().name(), "void");
}

NEW_TEST(ConstraintTest, MultipleIndependentStatements) {
    getDefaultTypeManager(tm);
    const auto T = CreateMultipleSymbols(tm, 12);
    const auto intType = tm.getRegisteredType("int");

    tm.CreateLiteralConformsToConstraint(T.at(3), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(6), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(7), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateBindToConstraint(T.at(0), intType);

    tm.CreateEqualsConstraint(T.at(3), T.at(4));
    tm.CreateEqualsConstraint(T.at(5), T.at(3));
    tm.CreateEqualsConstraint(T.at(6), T.at(7));
    tm.CreateEqualsConstraint(T.at(8), T.at(6));
    tm.CreateEqualsConstraint(T.at(9), T.at(8));
    tm.CreateEqualsConstraint(T.at(10), T.at(9));

    const auto funcHash = tm.CreateFunctionHash("foo", {});

    tm.CreateApplicableFunctionConstraint(funcHash, {}, T.at(0));
    tm.CreateBindFunctionConstraint(funcHash, T.at(11), {}, T.at(10));
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

NEW_TEST(ConstraintTest, MutuallyRecursiveSolveForLoopConstraints) {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 16);
    const auto intType = tm.getRegisteredType("int");
    const auto voidType = tm.getRegisteredType("void");
    const auto boolType = tm.getRegisteredType("bool");

    tm.CreateBindToConstraint(T.at(0), voidType);
    tm.CreateBindToConstraint(T.at(2), voidType);
    tm.CreateBindToConstraint(T.at(3), intType);
    tm.CreateBindToConstraint(T.at(8), boolType);
    tm.CreateBindToConstraint(T.at(12), voidType);
    tm.CreateBindToConstraint(T.at(13), intType);
    tm.CreateBindToConstraint(T.at(15), voidType);

    tm.CreateLiteralConformsToConstraint(T.at(4), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(6), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(10), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(T.at(14), typecheck::KnownProtocolKind::ExpressibleByInteger);

    tm.CreateEqualsConstraint(T.at(4), T.at(3));
    tm.CreateEqualsConstraint(T.at(5), T.at(1));
    tm.CreateEqualsConstraint(T.at(5), T.at(6));
    tm.CreateEqualsConstraint(T.at(7), T.at(5));
    tm.CreateEqualsConstraint(T.at(9), T.at(1));
    tm.CreateEqualsConstraint(T.at(9), T.at(10));
    tm.CreateEqualsConstraint(T.at(11), T.at(9));
    tm.CreateEqualsConstraint(T.at(3), T.at(11));
    tm.CreateEqualsConstraint(T.at(1), T.at(11));
    tm.CreateEqualsConstraint(T.at(14), T.at(13));

    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    for (std::size_t i = 0; i < 16; ++i) {
        CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(i)).has_generic());
    }
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(0)).generic().name() , "void");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(1)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(2)).generic().name() , "void");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(3)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(4)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(5)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(6)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(7)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(8)).generic().name() , "bool");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(9)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(10)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(11)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(12)).generic().name() , "void");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(13)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(14)).generic().name() , "int");
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(15)).generic().name() , "void");
}

NEW_TEST(ConstraintTest, RegressionTest1Constraints) {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 23);
    const auto intType = tm.getRegisteredType("int");
    const auto voidType = tm.getRegisteredType("void");
    const auto boolType = tm.getRegisteredType("bool");

//    T7: int
//    T10, T7
//    T11: ExpressibleByInt
//    T10, T11
//    T12, T10
//    T21, T7
//    T21, T22
//    T22: ExpressibleByInteger
    tm.CreateBindToConstraint(T.at(7), intType);
    tm.CreateEqualsConstraint(T.at(10), T.at(7));
    tm.CreateLiteralConformsToConstraint(T.at(11), typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(T.at(10), T.at(11));
    tm.CreateEqualsConstraint(T.at(12), T.at(10));
    tm.CreateEqualsConstraint(T.at(21), T.at(7));
    tm.CreateEqualsConstraint(T.at(21), T.at(22));
    tm.CreateLiteralConformsToConstraint(T.at(22), typecheck::KnownProtocolKind::ExpressibleByInteger);
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    CPPTEST_ASSERT_THAT(solution->GetResolvedType(T.at(11)).has_generic());
    CPPTEST_EXPECT_EQ(solution->GetResolvedType(T.at(11)).generic().name() , "int");
}

NEW_TEST(ConstraintTest, RegressionTest2ConstraintsAckerman) {
    getDefaultTypeManager(tm);
    tm.registerType("bool");
    tm.registerType("int");
    tm.registerType("void");

    const auto T = CreateMultipleSymbols(tm, 94);
    const auto intType = tm.getRegisteredType("int");
    const auto voidType = tm.getRegisteredType("void");
    const auto boolType = tm.getRegisteredType("bool");

    tm.CreateApplicableFunctionConstraint(6152725461566598243, { intType, intType }, intType);
    tm.CreateApplicableFunctionConstraint(-1993622415222145992, {}, intType);

	tm.CreateBindToConstraint(T.at(1), intType);
	tm.CreateBindToConstraint(T.at(2), voidType);
	tm.CreateBindToConstraint(T.at(3), intType);
	tm.CreateBindToConstraint(T.at(4), voidType);
	tm.CreateBindToConstraint(T.at(5), intType);
	tm.CreateEqualsConstraint(T.at(9), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(10), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(9), T.at(10));
	tm.CreateEqualsConstraint(T.at(11), T.at(9));
	tm.CreateBindToConstraint(T.at(12), boolType);
	tm.CreateEqualsConstraint(T.at(9), T.at(10));
	tm.CreateEqualsConstraint(T.at(13), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(14), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(13), T.at(14));
	tm.CreateEqualsConstraint(T.at(15), T.at(13));
	tm.CreateEqualsConstraint(T.at(16), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(17), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(16), T.at(17));
	tm.CreateEqualsConstraint(T.at(18), T.at(16));
	tm.CreateBindToConstraint(T.at(19), boolType);
	tm.CreateEqualsConstraint(T.at(16), T.at(17));
	tm.CreateEqualsConstraint(T.at(22), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(23), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(22), T.at(23));
	tm.CreateEqualsConstraint(T.at(24), T.at(22));
	tm.CreateEqualsConstraint(T.at(25), T.at(24));
	tm.CreateLiteralConformsToConstraint(T.at(26), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(27), T.at(26));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(21), {T.at(25),T.at(27)}, T.at(20));
	tm.CreateEqualsConstraint(T.at(28), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(29), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(28), T.at(29));
	tm.CreateEqualsConstraint(T.at(30), T.at(28));
	tm.CreateEqualsConstraint(T.at(31), T.at(30));
	tm.CreateLiteralConformsToConstraint(T.at(32), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(33), T.at(32));
	tm.CreateEqualsConstraint(T.at(36), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(37), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(36), T.at(37));
	tm.CreateEqualsConstraint(T.at(38), T.at(36));
	tm.CreateEqualsConstraint(T.at(39), T.at(38));
	tm.CreateEqualsConstraint(T.at(42), T.at(3));
	tm.CreateEqualsConstraint(T.at(43), T.at(42));
	tm.CreateEqualsConstraint(T.at(44), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(45), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(44), T.at(45));
	tm.CreateEqualsConstraint(T.at(46), T.at(44));
	tm.CreateEqualsConstraint(T.at(47), T.at(46));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(41), {T.at(43),T.at(47)}, T.at(40));
	tm.CreateEqualsConstraint(T.at(48), T.at(3));
	tm.CreateEqualsConstraint(T.at(49), T.at(48));
	tm.CreateEqualsConstraint(T.at(50), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(51), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(50), T.at(51));
	tm.CreateEqualsConstraint(T.at(52), T.at(50));
	tm.CreateEqualsConstraint(T.at(53), T.at(52));
	tm.CreateEqualsConstraint(T.at(54), T.at(40));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(35), {T.at(39),T.at(54)}, T.at(34));
	tm.CreateEqualsConstraint(T.at(55), T.at(3));
	tm.CreateLiteralConformsToConstraint(T.at(56), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(55), T.at(56));
	tm.CreateEqualsConstraint(T.at(57), T.at(55));
	tm.CreateEqualsConstraint(T.at(58), T.at(57));
	tm.CreateEqualsConstraint(T.at(61), T.at(3));
	tm.CreateEqualsConstraint(T.at(62), T.at(61));
	tm.CreateEqualsConstraint(T.at(63), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(64), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(63), T.at(64));
	tm.CreateEqualsConstraint(T.at(65), T.at(63));
	tm.CreateEqualsConstraint(T.at(66), T.at(65));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(60), {T.at(62),T.at(66)}, T.at(59));
	tm.CreateEqualsConstraint(T.at(67), T.at(3));
	tm.CreateEqualsConstraint(T.at(68), T.at(67));
	tm.CreateEqualsConstraint(T.at(69), T.at(5));
	tm.CreateLiteralConformsToConstraint(T.at(70), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(69), T.at(70));
	tm.CreateEqualsConstraint(T.at(71), T.at(69));
	tm.CreateEqualsConstraint(T.at(72), T.at(71));
	tm.CreateEqualsConstraint(T.at(73), T.at(59));
	tm.CreateBindToConstraint(T.at(74), boolType);
	tm.CreateEqualsConstraint(T.at(74), T.at(19));
	tm.CreateBindToConstraint(T.at(75), voidType);
	tm.CreateBindToConstraint(T.at(76), boolType);
	tm.CreateEqualsConstraint(T.at(76), T.at(12));
	tm.CreateBindToConstraint(T.at(77), voidType);
	tm.CreateLiteralConformsToConstraint(T.at(83), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(84), T.at(83));
	tm.CreateLiteralConformsToConstraint(T.at(85), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(86), T.at(85));
	tm.CreateBindFunctionConstraint(6152725461566598243, T.at(82), {T.at(84),T.at(86)}, T.at(81));
	tm.CreateLiteralConformsToConstraint(T.at(87), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(88), T.at(87));
	tm.CreateLiteralConformsToConstraint(T.at(89), typecheck::KnownProtocolKind::ExpressibleByInteger);
	tm.CreateEqualsConstraint(T.at(90), T.at(89));
	tm.CreateEqualsConstraint(T.at(91), T.at(81));
	tm.CreateEqualsConstraint(T.at(79), T.at(91));
	tm.CreateBindFunctionConstraint(-1993622415222145992, T.at(93), {}, T.at(92));

	const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

void RunStressTest(const std::size_t numSymbols) {
	getDefaultTypeManager(tm);
	tm.registerType("bool");
	tm.registerType("void");

	const auto T = CreateMultipleSymbols(tm, numSymbols);
	const auto intType = tm.getRegisteredType("int");
	const auto voidType = tm.getRegisteredType("void");
	const auto boolType = tm.getRegisteredType("bool");

	for (std::size_t i = 0; i < numSymbols; ++i) {
		if (i % (numSymbols / 5) == 0) {
			tm.CreateBindToConstraint(T.at(i), intType);
		} else if (i % (numSymbols / 3) == 0) {
			tm.CreateLiteralConformsToConstraint(T.at(i), typecheck::KnownProtocolKind::ExpressibleByInteger);
		}

		tm.CreateEqualsConstraint(T.at(i), T.at((i + 1) % numSymbols));
	}
	const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
}

#define CREATE_STRESS_TEST(NUM_SYMBOLS) NEW_TEST(ConstraintTest, StressTest##NUM_SYMBOLS## Constraints) { RunStressTest(NUM_SYMBOLS); }

CREATE_STRESS_TEST(100)
CREATE_STRESS_TEST(200)
CREATE_STRESS_TEST(300)
CREATE_STRESS_TEST(400)
CREATE_STRESS_TEST(800)

// ArrayElement constraint tests
NEW_TEST(ConstraintTest, SolveSimpleArrayConstraint) {
    getDefaultTypeManager(tm);
    
    // Create: arrayVar = Array<elementVar>, elementVar = int
    auto arrayVar = tm.CreateTypeVar();
    auto elementVar = tm.CreateTypeVar();
    
    tm.CreateArrayElementConstraint(arrayVar, elementVar);
    tm.CreateBindToConstraint(elementVar, tm.getRegisteredType("int"));
    
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    
    // arrayVar should resolve to Array<int>
    const auto resolvedArray = solution->GetResolvedType(arrayVar);
    CPPTEST_ASSERT_THAT(resolvedArray.has_generic());
    CPPTEST_EXPECT_EQ(resolvedArray.generic().name(), "Array");
    CPPTEST_EXPECT_EQ(resolvedArray.generic().type_params_size(), 1);
    CPPTEST_ASSERT_THAT(resolvedArray.generic().type_params(0).has_generic());
    CPPTEST_EXPECT_EQ(resolvedArray.generic().type_params(0).generic().name(), "int");
}

NEW_TEST(ConstraintTest, SolveArrayWithInferredElementType) {
    getDefaultTypeManager(tm);
    
    // Create: arrayVar = Array<elementVar>, 
    //         expr1Var = elementVar, expr2Var = elementVar
    //         expr1Var = int, expr2Var = int
    auto arrayVar = tm.CreateTypeVar();
    auto elementVar = tm.CreateTypeVar();
    auto expr1Var = tm.CreateTypeVar();
    auto expr2Var = tm.CreateTypeVar();
    
    tm.CreateArrayElementConstraint(arrayVar, elementVar);
    tm.CreateEqualsConstraint(expr1Var, elementVar);
    tm.CreateEqualsConstraint(expr2Var, elementVar);
    tm.CreateBindToConstraint(expr1Var, tm.getRegisteredType("int"));
    tm.CreateBindToConstraint(expr2Var, tm.getRegisteredType("int"));
    
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    
    const auto resolvedArray = solution->GetResolvedType(arrayVar);
    CPPTEST_EXPECT_EQ(resolvedArray.generic().name(), "Array");
    CPPTEST_EXPECT_EQ(resolvedArray.generic().type_params(0).generic().name(), "int");
}

NEW_TEST(ConstraintTest, SolveNestedArray) {
    getDefaultTypeManager(tm);
    
    // Array<Array<int>>
    auto outerArrayVar = tm.CreateTypeVar();
    auto innerArrayVar = tm.CreateTypeVar();
    auto elementVar = tm.CreateTypeVar();
    
    tm.CreateArrayElementConstraint(outerArrayVar, innerArrayVar);
    tm.CreateArrayElementConstraint(innerArrayVar, elementVar);
    tm.CreateBindToConstraint(elementVar, tm.getRegisteredType("int"));
    
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    
    const auto resolvedOuter = solution->GetResolvedType(outerArrayVar);
    CPPTEST_EXPECT_EQ(resolvedOuter.generic().name(), "Array");
    CPPTEST_ASSERT_THAT(resolvedOuter.generic().type_params(0).has_generic());
    CPPTEST_EXPECT_EQ(resolvedOuter.generic().type_params(0).generic().name(), "Array");
    CPPTEST_ASSERT_THAT(resolvedOuter.generic().type_params(0).generic().type_params(0).has_generic());
    CPPTEST_EXPECT_EQ(resolvedOuter.generic().type_params(0).generic().type_params(0).generic().name(), "int");
}

NEW_TEST(ConstraintTest, SolveArrayFromLiteralElements) {
    getDefaultTypeManager(tm);
    
    // Simulating [1, 2, 3] -> Array<int>
    auto arrayVar = tm.CreateTypeVar();
    auto elementVar = tm.CreateTypeVar();
    auto literal1 = tm.CreateTypeVar();
    auto literal2 = tm.CreateTypeVar();
    auto literal3 = tm.CreateTypeVar();
    
    tm.CreateArrayElementConstraint(arrayVar, elementVar);
    tm.CreateLiteralConformsToConstraint(literal1, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(literal2, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateLiteralConformsToConstraint(literal3, typecheck::KnownProtocolKind::ExpressibleByInteger);
    tm.CreateEqualsConstraint(literal1, elementVar);
    tm.CreateEqualsConstraint(literal2, elementVar);
    tm.CreateEqualsConstraint(literal3, elementVar);
    
    const auto solution = tm.solve();
    CPPTEST_ASSERT_THAT(solution.has_value());
    
    const auto resolvedArray = solution->GetResolvedType(arrayVar);
    CPPTEST_EXPECT_EQ(resolvedArray.generic().name(), "Array");
    CPPTEST_EXPECT_EQ(resolvedArray.generic().type_params(0).generic().name(), "int");
}
