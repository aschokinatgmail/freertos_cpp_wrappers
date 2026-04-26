#include <gtest/gtest.h>

#include <freertos_fixed_function.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace {

using freertos::fixed_function;

int free_fn(int x) { return x * 2; }

void void_fn() {}

struct Counter {
    int count = 0;
    int operator()() {
        ++count;
        return count;
    }
};

struct ConstCallable {
    int val;
    int operator()() const { return val; }
};

struct Adder {
    int offset;
    int operator()(int x) const { return x + offset; }
};

struct MoveOnlyCallable {
    int val;
    MoveOnlyCallable() : val(0) {}
    explicit MoveOnlyCallable(int v) : val(v) {}
    MoveOnlyCallable(const MoveOnlyCallable&) = delete;
    MoveOnlyCallable& operator=(const MoveOnlyCallable&) = delete;
    MoveOnlyCallable(MoveOnlyCallable&& o) noexcept : val(o.val) { o.val = 0; }
    MoveOnlyCallable& operator=(MoveOnlyCallable&& o) noexcept {
        val = o.val;
        o.val = 0;
        return *this;
    }
    int operator()() { return ++val; }
};

TEST(FixedFunctionTest, DefaultConstructionIsEmpty) {
    fixed_function<int()> f;
    EXPECT_FALSE(f);
    EXPECT_TRUE(!f);
}

TEST(FixedFunctionTest, NullptrConstructionIsEmpty) {
    fixed_function<int()> f(nullptr);
    EXPECT_FALSE(f);
}

TEST(FixedFunctionTest, FunctionPointerConstructionAndCall) {
    fixed_function<int(int)> f(free_fn);
    EXPECT_TRUE(f);
    EXPECT_EQ(f(3), 6);
}

TEST(FixedFunctionTest, FunctionPointerVoidReturn) {
    fixed_function<void()> f(void_fn);
    EXPECT_TRUE(f);
    f();
}

TEST(FixedFunctionTest, LambdaNoCapture) {
    fixed_function<int(int)> f([](int x) { return x + 1; });
    EXPECT_TRUE(f);
    EXPECT_EQ(f(10), 11);
}

TEST(FixedFunctionTest, LambdaWithCapture) {
    int offset = 42;
    fixed_function<int(int)> f([offset](int x) { return x + offset; });
    EXPECT_TRUE(f);
    EXPECT_EQ(f(8), 50);
}

TEST(FixedFunctionTest, LambdaMutableCapture) {
    fixed_function<int()> f([count = 0]() mutable { return ++count; });
    EXPECT_TRUE(f);
    EXPECT_EQ(f(), 1);
    EXPECT_EQ(f(), 2);
}

TEST(FixedFunctionTest, FunctorWithState) {
    Counter c;
    fixed_function<int()> f(c);
    EXPECT_TRUE(f);
    EXPECT_EQ(f(), 1);
    EXPECT_EQ(f(), 2);
}

TEST(FixedFunctionTest, FunctorWithMember) {
    Adder a{10};
    fixed_function<int(int)> f(a);
    EXPECT_TRUE(f);
    EXPECT_EQ(f(5), 15);
}

TEST(FixedFunctionTest, CopyConstruction) {
    int val = 7;
    fixed_function<int(int)> f1([val](int x) { return x + val; });
    fixed_function<int(int)> f2(f1);
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(3), 10);
}

TEST(FixedFunctionTest, CopyConstructionFromEmpty) {
    fixed_function<int(int)> f1;
    fixed_function<int(int)> f2(f1);
    EXPECT_FALSE(f2);
}

TEST(FixedFunctionTest, CopyConstructionFunctionPointer) {
    fixed_function<int(int)> f1(free_fn);
    fixed_function<int(int)> f2(f1);
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(5), 10);
}

TEST(FixedFunctionTest, CopyAssignment) {
    int val = 7;
    fixed_function<int(int)> f1([val](int x) { return x + val; });
    fixed_function<int(int)> f2;
    f2 = f1;
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(3), 10);
}

TEST(FixedFunctionTest, MoveConstruction) {
    int val = 5;
    fixed_function<int(int)> f1([val](int x) { return x * val; });
    fixed_function<int(int)> f2(std::move(f1));
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(3), 15);
}

TEST(FixedFunctionTest, MoveConstructionFromEmpty) {
    fixed_function<int(int)> f1;
    fixed_function<int(int)> f2(std::move(f1));
    EXPECT_FALSE(f2);
}

TEST(FixedFunctionTest, MoveAssignment) {
    int val = 5;
    fixed_function<int(int)> f1([val](int x) { return x * val; });
    fixed_function<int(int)> f2;
    f2 = std::move(f1);
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(3), 15);
}

TEST(FixedFunctionTest, MoveOnlyCallable) {
    fixed_function<int()> f(MoveOnlyCallable(99));
    EXPECT_TRUE(f);
    EXPECT_EQ(f(), 100);
    EXPECT_EQ(f(), 101);
}

TEST(FixedFunctionTest, MoveOnlyCallableMoveConstruction) {
    fixed_function<int()> f1(MoveOnlyCallable(42));
    fixed_function<int()> f2(std::move(f1));
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(), 43);
}

TEST(FixedFunctionTest, MoveOnlyCallableCopyYieldsEmpty) {
    fixed_function<int()> f1(MoveOnlyCallable(42));
    fixed_function<int()> f2(f1);
    EXPECT_FALSE(f2);
}

TEST(FixedFunctionTest, AssignNullptr) {
    fixed_function<int(int)> f(free_fn);
    EXPECT_TRUE(f);
    f = nullptr;
    EXPECT_FALSE(f);
}

TEST(FixedFunctionTest, SwapTwoNonEmpty) {
    int v1 = 10, v2 = 20;
    fixed_function<int(int)> f1([v1](int x) { return x + v1; });
    fixed_function<int(int)> f2([v2](int x) { return x + v2; });
    f1.swap(f2);
    EXPECT_EQ(f1(1), 21);
    EXPECT_EQ(f2(1), 11);
}

TEST(FixedFunctionTest, SwapEmptyAndNonEmpty) {
    int v = 5;
    fixed_function<int(int)> f1;
    fixed_function<int(int)> f2([v](int x) { return x * v; });
    f1.swap(f2);
    EXPECT_TRUE(f1);
    EXPECT_FALSE(f2);
    EXPECT_EQ(f1(3), 15);
}

TEST(FixedFunctionTest, SwapViaAdl) {
    int v1 = 3, v2 = 7;
    fixed_function<int(int)> f1([v1](int x) { return x * v1; });
    fixed_function<int(int)> f2([v2](int x) { return x + v2; });
    using std::swap;
    swap(f1, f2);
    EXPECT_EQ(f1(2), 9);
    EXPECT_EQ(f2(2), 6);
}

TEST(FixedFunctionTest, SelfSwap) {
    int v = 42;
    fixed_function<int(int)> f([v](int x) { return x + v; });
    f.swap(f);
    EXPECT_TRUE(f);
    EXPECT_EQ(f(1), 43);
}

TEST(FixedFunctionTest, EqualityWithNullptr) {
    fixed_function<int()> f;
    EXPECT_TRUE(f == nullptr);
    EXPECT_TRUE(nullptr == f);
    EXPECT_FALSE(f != nullptr);
    EXPECT_FALSE(nullptr != f);

    fixed_function<int(int)> g(free_fn);
    EXPECT_FALSE(g == nullptr);
    EXPECT_FALSE(nullptr == g);
    EXPECT_TRUE(g != nullptr);
    EXPECT_TRUE(nullptr != g);
}

TEST(FixedFunctionTest, OperatorBoolNodiscard) {
    fixed_function<void()> f;
    EXPECT_FALSE(static_cast<bool>(f));
    f = void_fn;
    EXPECT_TRUE(static_cast<bool>(f));
}

TEST(FixedFunctionTest, CustomCapacity) {
    fixed_function<int(), 128> f([]() { return 42; });
    EXPECT_TRUE(f);
    EXPECT_EQ(f(), 42);
}

TEST(FixedFunctionTest, CopyFunctionPointerThenModify) {
    fixed_function<int(int)> f1(free_fn);
    fixed_function<int(int)> f2(f1);
    EXPECT_EQ(f1(5), 10);
    EXPECT_EQ(f2(5), 10);
    f1 = nullptr;
    EXPECT_FALSE(f1);
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(5), 10);
}

TEST(FixedFunctionTest, MultipleCallSameInstance) {
    int sum = 0;
    fixed_function<void(int)> f([&sum](int x) { sum += x; });
    f(1);
    f(2);
    f(3);
    EXPECT_EQ(sum, 6);
}

TEST(FixedFunctionTest, ReassignFromEmptyToNonEmpty) {
    fixed_function<int(int)> f;
    EXPECT_FALSE(f);
    f = fixed_function<int(int)>(free_fn);
    EXPECT_TRUE(f);
    EXPECT_EQ(f(5), 10);
}

TEST(FixedFunctionTest, ReassignFromNonEmptyToNonEmpty) {
    int v1 = 10, v2 = 20;
    fixed_function<int(int)> f([v1](int x) { return x + v1; });
    fixed_function<int(int)> g([v2](int x) { return x + v2; });
    f = std::move(g);
    EXPECT_EQ(f(5), 25);
}

TEST(FixedFunctionTest, ConstCall) {
    ConstCallable c{42};
    fixed_function<int()> f(c);
    const auto& cf = f;
    EXPECT_EQ(cf(), 42);
}

TEST(FixedFunctionTest, VoidReturnType) {
    int counter = 0;
    fixed_function<void()> f([&counter]() { counter++; });
    f();
    f();
    EXPECT_EQ(counter, 2);
}

TEST(FixedFunctionTest, DestroyAndReassign) {
    int val = 5;
    fixed_function<int(int)> f([val](int x) { return x * val; });
    EXPECT_EQ(f(3), 15);
    int val2 = 10;
    f = [val2](int x) { return x + val2; };
    EXPECT_EQ(f(3), 13);
}

TEST(FixedFunctionTest, MoveAssignToActive) {
    int v1 = 1, v2 = 2;
    fixed_function<int()> f1([v1]() { return v1; });
    fixed_function<int()> f2([v2]() { return v2; });
    f2 = std::move(f1);
    EXPECT_TRUE(f2);
    EXPECT_EQ(f2(), 1);
}

TEST(FixedFunctionTest, SwapFunctionPointers) {
    fixed_function<int(int)> f1(free_fn);
    fixed_function<int(int)> f2([](int x) { return x + 10; });
    f1.swap(f2);
    EXPECT_EQ(f1(5), 15);
    EXPECT_EQ(f2(5), 10);
}

TEST(FixedFunctionTest, SizeMatchesDefaultCapacity) {
    static_assert(sizeof(fixed_function<void()>) >= 4 * sizeof(void*),
                  "Default capacity should be 4 * sizeof(void*)");
    static_assert(sizeof(fixed_function<void(), 16>) >= 16,
                  "Custom capacity should be respected");
}

} // namespace