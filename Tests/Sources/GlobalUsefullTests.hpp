//
// Created by ianpo on 15/05/2025.
//

#pragma once

#include <Imagine/Core.hpp>
#include <gtest/gtest.h>

using namespace Imagine::Literal;
using namespace Imagine::Core;
using namespace Imagine;

#ifdef MGN_DOUBLE

	#ifndef EXPECT_REAL_EQ
		#define EXPECT_REAL_EQ(val1, val2) EXPECT_DOUBLE_EQ(val1, val2)
	#endif


	#ifndef ASSERT_REAL_EQ
		#define ASSERT_REAL_EQ(val1, val2) ASSERT_DOUBLE_EQ(val1, val2)
	#endif

#else

	#ifndef EXPECT_REAL_EQ
		#define EXPECT_REAL_EQ(val1, val2) EXPECT_FLOAT_EQ(val1, val2)
	#endif


	#ifndef ASSERT_REAL_EQ
		#define ASSERT_REAL_EQ(val1, val2) ASSERT_FLOAT_EQ(val1, val2)
	#endif

#endif