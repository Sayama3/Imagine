//
// Created by ianpo on 15/05/2025.
//

#include "GlobalUsefullTests.hpp"

TEST(CoreHeapArray, Redimensionning) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	ScopePtr<HeapArray<Real>> heapArray = ScopePtr<HeapArray<Real>>::Create();
	ASSERT_FALSE(heapArray->is_valid());

	heapArray->resize(10);
	ASSERT_TRUE(heapArray->is_valid());
	ASSERT_EQ(heapArray->size(), 10);
	ASSERT_EQ(heapArray->capacity(), 10);

	heapArray->redimension(5);
	ASSERT_TRUE(heapArray->is_valid());
	ASSERT_EQ(heapArray->size(), 5);
	ASSERT_EQ(heapArray->capacity(), 10);

	heapArray->resize(5);
	ASSERT_TRUE(heapArray->is_valid());
	ASSERT_EQ(heapArray->size(), 5);
	ASSERT_EQ(heapArray->capacity(), 5);

	heapArray.Release();
	Log::Shutdown();
}

TEST(CoreHeapArray, PushPopRemoveAssertions) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	HeapArray<int> heapArray{1};
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 1);
	ASSERT_TRUE(heapArray.empty());

	heapArray.push_back(-1);
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_FALSE(heapArray.empty());
	ASSERT_EQ(heapArray.back(), -1);
	ASSERT_EQ(heapArray.get(0), -1);
	ASSERT_EQ(heapArray.size(), 1);
	ASSERT_EQ(heapArray.capacity(), 1);

	heapArray.emplace_back();
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 2);
	ASSERT_EQ(heapArray.capacity(), 2);

	heapArray.back() = 1;
	ASSERT_EQ(heapArray.back(), 1);
	ASSERT_EQ(heapArray.get(1), 1);

	heapArray.pop_back();
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 1);
	ASSERT_EQ(heapArray.capacity(), 2);
	ASSERT_EQ(heapArray.back(), -1);
	ASSERT_EQ(heapArray.get(0), -1);

	heapArray.pop_back();
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 2);

	heapArray.reserve(6);
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 6);
	for (int i = 0; i < 5; ++i) {
		const int n = i+1;
		heapArray.push_back(n);
		ASSERT_EQ(heapArray.size(), n);
		ASSERT_EQ(heapArray.back(), n);
		ASSERT_EQ(heapArray.get(i), n);
	}

	heapArray.prepare(5);
	ASSERT_EQ(heapArray.size(), 5);
	ASSERT_EQ(heapArray.capacity(), 10);
	for (int i = 5; i < 10; ++i) {
		const int n = i+1;
		heapArray.push_back(n);
		ASSERT_EQ(heapArray.size(), n);
		ASSERT_EQ(heapArray.back(), n);
		ASSERT_EQ(heapArray.get(i), n);
	}

	heapArray.remove(0);
	ASSERT_EQ(heapArray.size(), 9);
	ASSERT_EQ(heapArray.capacity(), 10);
	ASSERT_EQ(heapArray.back(), 10);
	ASSERT_EQ(heapArray.back(), heapArray.get(8));
	for (int i = 0; i < 9; ++i) {
		ASSERT_EQ(heapArray.get(i), i+2);
	}

	heapArray.swap_and_remove(3);
	ASSERT_EQ(heapArray.size(), 8);
	ASSERT_EQ(heapArray.capacity(), 10);
	ASSERT_EQ(heapArray.back(), 9);
	ASSERT_EQ(heapArray.get(3), 10);

	heapArray.clear();
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 10);
	ASSERT_TRUE(heapArray.empty());
	Log::Shutdown();
}

TEST(CoreRawHeapArray, Redimensionning) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	RawHeapArray<uint32_t> heapArray;
	ASSERT_FALSE(heapArray.is_valid());
	heapArray = RawHeapArray<uint32_t>{sizeof(int)};
	ASSERT_FALSE(heapArray.is_valid());

	heapArray.resize(10);
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 10);
	ASSERT_EQ(heapArray.capacity(), 10);

	heapArray.redimension(5);
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 5);
	ASSERT_EQ(heapArray.capacity(), 10);

	heapArray.resize(5);
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 5);
	ASSERT_EQ(heapArray.capacity(), 5);

	Log::Shutdown();
}

TEST(CoreRawHeapArray, PushPopRemoveAssertions) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	RawHeapArray<uint32_t> heapArray{sizeof(int), 1};
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 1);
	ASSERT_TRUE(heapArray.empty());

	heapArray.push_back(-1);
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_FALSE(heapArray.empty());
	ASSERT_EQ(*reinterpret_cast<int*>(heapArray.back()), -1);
	ASSERT_EQ(*reinterpret_cast<int*>(heapArray.get(0)), -1);
	ASSERT_EQ(heapArray.size(), 1);
	ASSERT_EQ(heapArray.capacity(), 1);

	heapArray.emplace_back();
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 2);
	ASSERT_EQ(heapArray.capacity(), 2);

	int data2 = 1;
	memcpy(heapArray.back(), &data2, sizeof(int));
	ASSERT_EQ(*static_cast<int*>(heapArray.back()), 1);
	ASSERT_EQ(*static_cast<int*>(heapArray.get(1)), 1);

	heapArray.pop_back();
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 1);
	ASSERT_EQ(heapArray.capacity(), 2);
	ASSERT_EQ(*static_cast<int*>(heapArray.back()), -1);
	ASSERT_EQ(*static_cast<int*>(heapArray.get(0)), -1);

	heapArray.pop_back();
	ASSERT_TRUE(heapArray.is_valid());
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 2);

	heapArray.reserve(6);
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 6);
	for (int i = 0; i < 5; ++i) {
		const int n = i+1;
		heapArray.push_back(&n, sizeof(int));
		ASSERT_EQ(heapArray.size(), n);
		ASSERT_EQ(*static_cast<int*>(heapArray.back()), n);
		ASSERT_EQ(*static_cast<int*>(heapArray.get(i)), n);
	}

	heapArray.prepare(5);
	ASSERT_EQ(heapArray.size(), 5);
	ASSERT_EQ(heapArray.capacity(), 10);
	for (int i = 5; i < 10; ++i) {
		const int n = i+1;
		heapArray.push_back(&n, sizeof(int));
		ASSERT_EQ(heapArray.size(), n);
		ASSERT_EQ(*static_cast<int*>(heapArray.back()), n);
		ASSERT_EQ(*static_cast<int*>(heapArray.get(i)), n);
	}

	heapArray.remove(0);
	ASSERT_EQ(heapArray.size(), 9);
	ASSERT_EQ(heapArray.capacity(), 10);
	ASSERT_EQ(*static_cast<int*>(heapArray.back()), 10);
	ASSERT_EQ(*static_cast<int*>(heapArray.back()), *static_cast<int*>(heapArray.get(8)));
	for (int i = 0; i < 9; ++i) {
		ASSERT_EQ(*static_cast<int*>(heapArray.get(i)), i+2);
	}

	heapArray.swap_and_remove(3);
	ASSERT_EQ(heapArray.size(), 8);
	ASSERT_EQ(heapArray.capacity(), 10);
	ASSERT_EQ(*static_cast<int*>(heapArray.back()), 9);
	ASSERT_EQ(*static_cast<int*>(heapArray.get(3)), 10);

	heapArray.clear();
	ASSERT_EQ(heapArray.size(), 0);
	ASSERT_EQ(heapArray.capacity(), 10);
	ASSERT_TRUE(heapArray.empty());

	Log::Shutdown();
}