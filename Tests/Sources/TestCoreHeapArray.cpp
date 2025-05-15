//
// Created by ianpo on 15/05/2025.
//

#include <Imagine/Core.hpp>
#include <gtest/gtest.h>

using namespace Imagine::Literal;
using namespace Imagine;

#ifdef MGN_DOUBLE
	#define EXPECT_REAL_EQ(val1, val2) EXPECT_DOUBLE_EQ(val1, val2)
	#define ASSERT_REAL_EQ(val1, val2) ASSERT_DOUBLE_EQ(val1, val2)
#else
	#define EXPECT_REAL_EQ(val1, val2) EXPECT_FLOAT_EQ(val1, val2)
	#define ASSERT_REAL_EQ(val1, val2) ASSERT_FLOAT_EQ(val1, val2)
#endif

// Demonstrate some basic assertions.
TEST(CoreBasicsTests, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_REAL_EQ(7_r * 6_r, 42.0_r);
}
