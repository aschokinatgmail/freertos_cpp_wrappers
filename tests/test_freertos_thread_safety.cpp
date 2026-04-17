#include "freertos_thread_safety.hpp"
#include <gtest/gtest.h>

class FREERTOS_CAPABILITY("mutex") test_capability {
public:
  void lock() FREERTOS_ACQUIRE("mutex") {}
  void unlock() FREERTOS_RELEASE("mutex") {}
  bool try_lock() FREERTOS_TRY_ACQUIRE("mutex") { return true; }
  int data FREERTOS_GUARDED_BY("mutex");
};

class FREERTOS_SCOPED_CAPABILITY test_scoped_capability {
public:
  void lock() FREERTOS_ACQUIRE("mutex") {}
  void unlock() FREERTOS_RELEASE("mutex") {}
};

TEST(ThreadSafetyAnnotationTest, MacrosExpandToNothingByDefault) {
#ifndef FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS
  SUCCEED();
#else
  EXPECT_EQ(FREERTOS_CPP_WRAPPERS_THREAD_SAFETY_ANNOTATIONS, 0);
#endif
}

TEST(ThreadSafetyAnnotationTest, CapabilityAnnotationOnClass) {
  test_capability tc;
  tc.lock();
  tc.unlock();
  EXPECT_TRUE(tc.try_lock());
}

TEST(ThreadSafetyAnnotationTest, ScopedCapabilityAnnotation) {
  test_scoped_capability tsc;
  tsc.lock();
  tsc.unlock();
}

TEST(ThreadSafetyAnnotationTest, FunctionAnnotationsCompile) {
  test_capability tc;
  tc.lock();
  int val FREERTOS_GUARDED_BY("mutex") = 0;
  (void)val;
  EXPECT_EQ(val, 0);
}

TEST(ThreadSafetyAnnotationTest, RequiresAnnotation) {
  test_capability tc;
  tc.lock();
  tc.unlock();
}

TEST(ThreadSafetyAnnotationTest, ExcludesAnnotation) {
  FREERTOS_EXCLUDES("mutex")
  EXPECT_TRUE(true);
}

TEST(ThreadSafetyAnnotationTest, NoThreadSafetyAnalysis) {
  FREERTOS_NO_THREAD_SAFETY_ANALYSIS
  EXPECT_TRUE(true);
}