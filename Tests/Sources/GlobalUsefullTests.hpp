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
class AtomicInstanceCount {
public:
	inline static std::atomic_uint32_t s_InstanceCount{0};

	AtomicInstanceCount() : data{} {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	template <typename... Args>
	inline static AtomicInstanceCount<T> Create(Args&&...args) {
		return AtomicInstanceCount<T>(T(std::forward<Args>(args)...));
	}

	AtomicInstanceCount(const T& other) : data{other} {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	~AtomicInstanceCount() {
		s_InstanceCount.fetch_sub(1, std::memory_order::relaxed);
	}

	AtomicInstanceCount(const AtomicInstanceCount& other) : data(other.data) {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	AtomicInstanceCount& operator=(const AtomicInstanceCount& other) {
		data = other.data;
		return *this;
	}

	AtomicInstanceCount(AtomicInstanceCount&& other) noexcept : data(other.data) {
		s_InstanceCount.fetch_add(1, std::memory_order::relaxed);
	}

	AtomicInstanceCount& operator=(AtomicInstanceCount&& other) noexcept {
		data = other.data;
		return *this;
	};

	void swap(AtomicInstanceCount& other) noexcept {
		std::swap(data, other.data);
	}

	T data{0};
};

template<typename T>
class InstanceCount {
public:
	inline static volatile std::uint32_t s_InstanceCount{0};

	InstanceCount() : data{} {
		s_InstanceCount += 1;
	}

	template <typename... Args>
	inline static InstanceCount<T> Create(Args&&...args) {
		return InstanceCount<T>(T(std::forward<Args>(args)...));
	}

	InstanceCount(const T& other) : data{other} {
		s_InstanceCount += 1;
	}

	~InstanceCount() {
		s_InstanceCount -= 1;
	}

	InstanceCount(const InstanceCount& other) : data(other.data) {
		s_InstanceCount += 1;
	}

	InstanceCount& operator=(const InstanceCount& other) {
		data = other.data;
		return *this;
	}

	InstanceCount(InstanceCount&& other) noexcept {
		s_InstanceCount += 1;
		swap(other);
	}

	InstanceCount& operator=(InstanceCount&& other) noexcept {
		swap(other);
		return *this;
	};

	void swap(InstanceCount& other) noexcept {
		std::swap(data, other.data);
	}

	T data{0};
};


template<typename T>
class ScopePtr {
public:
	template<typename... Args>
	inline static ScopePtr<T> Create(Args&&... args) {
		return ScopePtr(new T(std::forward<Args>(args)...));
	}
public:
	ScopePtr() : ptr(nullptr) {}
	ScopePtr(T* ptr) : ptr(ptr) {}
	~ScopePtr() {Release();}

	ScopePtr(const ScopePtr&) = delete;
	ScopePtr& operator=(const ScopePtr&) = delete;

	ScopePtr(ScopePtr&& other) noexcept {swap(other);}
	ScopePtr& operator=(ScopePtr&& other) noexcept {Set(other); return *this;}

	ScopePtr* operator=(T* ptr) {
		Set(ptr);
		return *this;
	}

	[[nodiscard]] T* operator->() {return ptr;}
	[[nodiscard]] const T* operator->() const {return ptr;}
	[[nodiscard]] explicit operator bool() const {return ptr != nullptr;}

	void swap(ScopePtr& other) noexcept {
		std::swap(ptr, other.ptr);
	}

	void Set(T* ptr) {
		Release();
		this->ptr = ptr;
	}

	void Set(ScopePtr<T>&& ptr) {
		Release();
		swap(ptr);
	}

	void Reallocate() {
		Release();
		ptr = new T();
	}

	void Release() {
		delete ptr;
		ptr = nullptr;
	}

	T* ptr{nullptr};
};