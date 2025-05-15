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


template<typename T>
class TmpAtomicInstanceCount {
public:
	inline static volatile std::atomic_uint32_t s_InstanceCount{0};

	TmpAtomicInstanceCount() {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	TmpAtomicInstanceCount(const T& data) : data(data) {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	~TmpAtomicInstanceCount() {
		s_InstanceCount.fetch_sub(1, std::memory_order::relaxed);
	}

	TmpAtomicInstanceCount(const TmpAtomicInstanceCount& other) : data(other.data) {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	TmpAtomicInstanceCount& operator=(const TmpAtomicInstanceCount& other) {
		data = other.data;
		return *this;
	}

	TmpAtomicInstanceCount(TmpAtomicInstanceCount&& other) noexcept : data(other.data) {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	TmpAtomicInstanceCount& operator=(TmpAtomicInstanceCount&& other) noexcept {
		data = other.data;
		return *this;
	};

	void swap(TmpAtomicInstanceCount& other) {
		std::swap(data, other.data);
	}

	T data{0};
};

template<typename T>
class TmpInstanceCount {
public:
	inline static volatile std::uint32_t s_InstanceCount{0};

	TmpInstanceCount() {
		s_InstanceCount += 1;
	}

	TmpInstanceCount(const T& data) : data(data) {
		s_InstanceCount += 1;
	}

	~TmpInstanceCount() {
		s_InstanceCount -= 1;
	}

	TmpInstanceCount(const TmpInstanceCount& other) : data(other.data) {
		s_InstanceCount += 1;
	}

	TmpInstanceCount& operator=(const TmpInstanceCount& other) {
		data = other.data;
		return *this;
	}

	TmpInstanceCount(TmpInstanceCount&& other) noexcept {
		s_InstanceCount += 1;
		swap(other);
	}

	TmpInstanceCount& operator=(TmpInstanceCount&& other) noexcept {
		swap(other);
		return *this;
	};

	void swap(TmpInstanceCount& other) noexcept {
		std::swap(data, other.data);
	}

	T data{0};
};