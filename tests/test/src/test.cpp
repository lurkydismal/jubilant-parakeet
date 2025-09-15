#include "test.hpp"

#include "stdfunc.hpp"

namespace {

auto g_array1 =
    stdfunc::makeVariantContainer< std::array >( true,
                                                 std::string_view( " true " ),
                                                 123,
                                                 "",
                                                 'a' );
auto g_array2 =
    stdfunc::makeVariantContainer< std::array >( false,
                                                 std::string_view( " false " ),
                                                 321,
                                                 " ",
                                                 'b' );

} // namespace

TEST( test, EXPECT_TRUE ) {
    EXPECT_TRUE( true );
    EXPECT_TRUE( 1 );
    EXPECT_TRUE( !0 );
    EXPECT_TRUE( "" );
}

TEST( test, EXPECT_FALSE ) {
    EXPECT_FALSE( false );
    EXPECT_FALSE( 0 );
    EXPECT_FALSE( int{ -1 } );
    EXPECT_FALSE( !1 );
}

TEST( test, EXPECT_EQ ) {
    for ( const auto& _element : g_array1 ) {
        std::visit( []( auto&& _value ) { EXPECT_EQ( _value, _value ); },
                    _element );
    }
}

TEST( test, EXPECT_NE ) {
    for ( const auto& [ _element1, _element2 ] :
          std::views::zip( g_array1, g_array2 ) ) {
        std::visit(
            []( auto&& _value1, auto&& _value2 ) {
                if constexpr ( std::equality_comparable_with<
                                   decltype( _value1 ),
                                   decltype( _value2 ) > ) {
                    EXPECT_NE( _value1, _value2 );
                }
            },
            _element1, _element2 );
    }
}

#if 0
// gtest_feature_tests.cpp
#include <gmock/gmock.h> // matchers like ElementsAre, HasSubstr, MatchesRegex
#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using ::testing::ElementsAre;
using ::testing::UnorderedElementsAre;
using ::testing::Contains;
using ::testing::HasSubstr;
using ::testing::MatchesRegex;
using ::testing::Each;
using ::testing::SizeIs;
using ::testing::Gt;
using ::testing::Eq;

//
// Basic assertions
//
TEST(BasicAssertions, TrueFalseAndNull) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);

    // non-zero integral is true
    EXPECT_TRUE(1);
    EXPECT_FALSE(0);

    // pointer checks
    int x = 5;
    int* p = &x;
    EXPECT_NE(nullptr, p);
    EXPECT_EQ(nullptr, nullptr);
}

TEST(ComparisonAssertions, EqualityAndOrdering) {
    EXPECT_EQ(5, 2 + 3);
    EXPECT_NE(5, 2 + 2);
    EXPECT_LT(1, 2);
    EXPECT_LE(2, 2);
    EXPECT_GT(3, 2);
    EXPECT_GE(3, 3);
}

TEST(StringAssertions, CStyleAndStdString) {
    const char* s1 = "hello";
    const char* s2 = "hello";
    std::string ss = "hello world";

    EXPECT_STREQ(s1, s2);        // C strings
    EXPECT_STRNE(s1, "goodbye");
    EXPECT_PRED_FORMAT2(::testing::PrintToStringPredicateFormat<std::string, std::string>,
                        ss.substr(0,5), "hello"); // demonstrate predicate format use (not typical)
    EXPECT_THAT(ss, HasSubstr("world")); // needs gmock
}

TEST(FloatingAssertions, NearAndNan) {
    double a = 0.1 + 0.2;
    // approximate comparison
    EXPECT_NEAR(a, 0.3, 1e-12);

    double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_TRUE(std::isnan(nan));
}

//
// Exception assertions
//
void throw_runtime() { throw std::runtime_error("bad"); }

TEST(ExceptionAssertions, ThrowAndNoThrow) {
    EXPECT_THROW(throw_runtime(), std::runtime_error);
    EXPECT_ANY_THROW([](){ throw 1; }());
    EXPECT_NO_THROW(int y = 1 + 1; (void)y);
}

//
// Fatal vs Non-Fatal demonstration
//
TEST(FatalNonFatal, ExpectVsAssert) {
    EXPECT_EQ(1, 2) << "This EXPECT fails but test continues";
    // Next line is an example of ASSERT which would abort test body if failed
    ASSERT_EQ(2, 2) << "This ASSERT succeeds and allows continuation";
    SUCCEED() << "Reached after ASSERT";
}

//
// Capture stdout/stderr
//
TEST(CaptureStdout, CaptureAndCheck) {
    testing::internal::CaptureStdout();
    std::cout << "hello capture";
    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_THAT(output, HasSubstr("hello capture"));
}

//
// Test fixture with SetUp/TearDown + TestSuite SetUp/TearDown
//
class FixtureExample : public testing::Test {
protected:
    static inline std::vector<int> s_shared;

    static void SetUpTestSuite() {
        // called once before all tests in this suite
        s_shared = {1,2,3};
    }
    static void TearDownTestSuite() {
        s_shared.clear();
    }

    void SetUp() override {
        // called before each test
        l_local = 100;
    }
    void TearDown() override {
        // called after each test
    }

    int l_local = 0;
};

TEST_F(FixtureExample, UsesLocalAndShared) {
    EXPECT_EQ(l_local, 100);
    EXPECT_THAT(s_shared, ElementsAre(1,2,3));
}

TEST_F(FixtureExample, ModifyShared) {
    s_shared.push_back(4);
    EXPECT_THAT(s_shared, ElementsAre(1,2,3,4));
}

//
// Parameterized tests (value-parameterized)
//
class ParamTest : public testing::TestWithParam<int> {};

TEST_P(ParamTest, IsPositive) {
    int v = GetParam();
    EXPECT_GT(v, 0);
}

INSTANTIATE_TEST_SUITE_P(PositiveInts, ParamTest, testing::Values(1, 2, 3, 10));

//
// Parameterized test with tuple (multiple params)
//
using Pair = std::pair<int, int>;
class PairParamTest : public testing::TestWithParam<Pair> {};

TEST_P(PairParamTest, SumMatches) {
    auto [a,b] = GetParam();
    EXPECT_EQ(a + b, std::plus<int>()(a,b));
}

INSTANTIATE_TEST_SUITE_P(Pairs, PairParamTest,
                         testing::Values(std::make_pair(1,2),
                                         std::make_pair(3,4),
                                         std::make_pair(5,5)));

//
// Typed tests
//
template <typename T>
class TypedExample : public testing::Test {
public:
    using Type = T;
};

using MyTypes = ::testing::Types<int, double, std::string>;
TYPED_TEST_SUITE(TypedExample, MyTypes);

TYPED_TEST(TypedExample, IsDefaultConstructible) {
    TypeParam v{};
    (void)v;
    SUCCEED();
}

//
// Matchers examples (requires gmock)
//
TEST(Matchering, ContainerAndStringMatchers) {
    std::vector<int> v{1,2,3};
    EXPECT_THAT(v, ElementsAre(1,2,3));
    EXPECT_THAT(v, Each(Gt(0)));
    EXPECT_THAT(v, SizeIs(3u));

    std::string s = "abc def ghi";
    EXPECT_THAT(s, HasSubstr("def"));
    EXPECT_THAT(s, MatchesRegex(".*def.*"));
}

TEST(Matchering, UnorderedAndContains) {
    std::vector<int> v{3,1,2};
    EXPECT_THAT(v, UnorderedElementsAre(1,2,3));
    EXPECT_THAT(v, Contains(2));
}

//
// SCOPED_TRACE and failure context
//
TEST(Tracing, ScopedTraceMakesMessagesClearer) {
    for (int i=0;i<3;++i) {
        SCOPED_TRACE(testing::Message() << "iteration=" << i);
        EXPECT_LT(i, 5); // all pass but if they fail the trace helps
    }
}

//
// GTEST_SKIP and test properties
//
TEST(SkippingAndProperties, SkipAndRecord) {
    GTEST_SKIP() << "Skipping because example";
    // RecordProperty is useful for integration with CI/test-reporters
    RecordProperty("reason", "example skip");
    FAIL() << "This won't run because test was skipped";
}

//
// Multithreaded-ish test: user-spawned threads that gtest doesn't manage
//
TEST(UserThreads, WorkerThreadsDoWork) {
    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    for (int i=0;i<4;++i) {
        threads.emplace_back([&counter](){
            for (int k=0;k<1000;++k) ++counter;
        });
    }
    for (auto &t : threads) t.join();
    EXPECT_EQ(counter.load(), 4000);
}

//
// Death tests & exit tests (guarded). Requires gtest to support death tests.
#if GTEST_HAS_DEATH_TEST

void will_abort() {
    std::fflush(nullptr);
    abort(); // triggers SIGABRT
}

TEST(DeathTests, EXPECT_DEATH_Abort) {
    // regex ".*" matches any output
    EXPECT_DEATH(will_abort(), ".*");
}

void will_exit_code(int code) {
    std::exit(code);
}

TEST(DeathTests, EXPECT_EXIT_Code42) {
    // ExitedWithCode matcher checks exit status
    EXPECT_EXIT(will_exit_code(42), ::testing::ExitedWithCode(42), ".*");
}

#else
#pragma message( \
    "GTEST_HAS_DEATH_TEST not defined: death tests will be skipped" )
#endif // GTEST_HAS_DEATH_TEST

//
// EXPECT_EXIT with function object capturing output
//
#if GTEST_HAS_DEATH_TEST
void prints_and_exits() {
    std::cout << "bye world\n";
    std::exit(3);
}

TEST(ExitTests, PrintsAndExits) {
    EXPECT_EXIT(prints_and_exits(), ::testing::ExitedWithCode(3), HasSubstr("bye world"));
}
#endif

//
// Additional helpers/assertions demonstration
//
TEST(Additional, SucceedFailAddFailure) {
    SUCCEED() << "explicit success marker";
    ADD_FAILURE() << "explicit failure marker (non-fatal)";
    // Because ADD_FAILURE is non-fatal, the test continues and ends as failed.
    // For demonstration purposes we assert a condition to pass:
    EXPECT_TRUE(true);
}

//
// main() is typically provided by gtest_main; include only if you need a custom main.
// int main(int argc, char** argv) {
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }
#endif
