//
// Created by ianpo on 15/05/2025.
//

#include "GlobalUsefullTests.hpp"

TEST(CoreSparseSet, IntSparseSet) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	uint64_t idGenerator{0};
	{
		SparseSet<int, uint64_t> sparseSet{};
		ASSERT_EQ(sparseSet.Count(), 0);
	}
	SparseSet<int, uint64_t> sparseSet{32};
	ASSERT_EQ(sparseSet.Count(), 0);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	ASSERT_FALSE(sparseSet.Exist(0));
	ASSERT_FALSE(sparseSet.Exist(64));

	ASSERT_TRUE(sparseSet.Create(idGenerator++));
	ASSERT_FALSE(sparseSet.Create(0));
	ASSERT_TRUE(sparseSet.Exist(0));
	ASSERT_EQ(sparseSet.Get(0), int());
	ASSERT_EQ(sparseSet.Count(), 1);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	ASSERT_TRUE(sparseSet.Create(idGenerator++, 1));
	ASSERT_FALSE(sparseSet.Create(1));
	ASSERT_TRUE(sparseSet.Exist(1));
	ASSERT_EQ(sparseSet.Get(1), 1);
	ASSERT_EQ(sparseSet.Count(), 2);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	sparseSet.Remove(0);
	ASSERT_FALSE(sparseSet.Exist(0));
	ASSERT_TRUE(sparseSet.Exist(1));
	ASSERT_EQ(sparseSet.Get(1), 1);
	ASSERT_EQ(sparseSet.Count(), 1);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	ASSERT_TRUE(sparseSet.Create(0, -1));
	ASSERT_FALSE(sparseSet.Create(0));
	ASSERT_TRUE(sparseSet.Exist(0));
	ASSERT_EQ(sparseSet.Get(0), -1);
	ASSERT_EQ(sparseSet.Count(), 2);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	while (idGenerator < 32) {
		const auto data = idGenerator;
		ASSERT_TRUE(sparseSet.Create(idGenerator++, data));
		ASSERT_FALSE(sparseSet.Create(idGenerator - 1));
		ASSERT_TRUE(sparseSet.Exist(idGenerator - 1));
		ASSERT_EQ(sparseSet.Get(idGenerator - 1), data);
		ASSERT_EQ(sparseSet.Count(), idGenerator);
		ASSERT_EQ(sparseSet.Capacity(), 32);
	}

	ASSERT_TRUE(sparseSet.Create(128, 127));
	ASSERT_FALSE(sparseSet.Create(128));
	ASSERT_TRUE(sparseSet.Exist(128));
	ASSERT_EQ(sparseSet.Get(128), 127);
	ASSERT_EQ(sparseSet.Count(), idGenerator + 1);
	ASSERT_GE(sparseSet.Capacity(), 32);

	const auto new_capacity = sparseSet.Capacity();

	sparseSet.Clear();

	ASSERT_EQ(sparseSet.Count(), 0);
	ASSERT_EQ(sparseSet.Capacity(), new_capacity);
	Log::Shutdown();
}

TEST(CoreSparseSet, Vec4AutoSparseSet) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	using AutoSparseSet = AutoIdSparseSet<Vec4, uint64_t>;
	{
		AutoSparseSet sparseSet{};
		ASSERT_EQ(sparseSet.Count(), 0);
	}

	AutoSparseSet sparseSet{32};
	ASSERT_EQ(sparseSet.Count(), 0);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	ASSERT_FALSE(sparseSet.Exist(0));
	ASSERT_FALSE(sparseSet.Exist(64));

	const auto id1 = sparseSet.Create();
	ASSERT_NE(id1, (AutoSparseSet::NullId));
	ASSERT_FALSE(sparseSet.Create(id1));
	ASSERT_TRUE(sparseSet.Exist(id1));
	ASSERT_EQ(sparseSet.Get(id1), Vec4());
	ASSERT_EQ(sparseSet.Count(), 1);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	const auto id2 = sparseSet.Create(Vec4(1));
	ASSERT_FALSE(sparseSet.Create(id2));
	ASSERT_TRUE(sparseSet.Exist(id2));
	ASSERT_EQ(sparseSet.Get(id2), Vec4(1));
	ASSERT_EQ(sparseSet.Count(), 2);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	sparseSet.Remove(id1);
	ASSERT_FALSE(sparseSet.Exist(id1));
	ASSERT_TRUE(sparseSet.Exist(id2));
	ASSERT_EQ(sparseSet.Get(id2), Vec4(1));
	ASSERT_EQ(sparseSet.Count(), 1);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	ASSERT_TRUE(sparseSet.Create(id1, Vec4(-1)));
	ASSERT_FALSE(sparseSet.Create(id1));
	ASSERT_TRUE(sparseSet.Exist(id1));
	ASSERT_EQ(sparseSet.Get(id1), Vec4(-1));
	ASSERT_EQ(sparseSet.Count(), 2);
	ASSERT_EQ(sparseSet.Capacity(), 32);

	// It's available, the only one, so it should be filled.
	sparseSet.Remove(id1);
	ASSERT_EQ(sparseSet.Create(), id1);

	auto count = 2;
	while (sparseSet.Count() < 32) {
		const auto data = Vec4(sparseSet.Count() + 1);
		const auto id = sparseSet.Create(data);
		ASSERT_NE(id, (AutoSparseSet::NullId));
		ASSERT_FALSE(sparseSet.Create(id));
		ASSERT_TRUE(sparseSet.Exist(id));
		ASSERT_EQ(sparseSet.Get(id), data);
		ASSERT_EQ(sparseSet.Count(), ++count);
		ASSERT_EQ(sparseSet.Capacity(), 32);
	}

	const auto id3 = 128;
	const auto data3 = Vec4(127);
	ASSERT_NE(sparseSet.Create(id3, data3), AutoSparseSet::NullId);
	ASSERT_FALSE(sparseSet.Create(128));
	ASSERT_TRUE(sparseSet.Exist(128));
	ASSERT_EQ(sparseSet.Get(128), data3);
	ASSERT_EQ(sparseSet.Count(), 33);
	ASSERT_GE(sparseSet.Capacity(), 32);

	const auto new_capacity = sparseSet.Capacity();

	sparseSet.Clear();

	ASSERT_EQ(sparseSet.Count(), 0);
	ASSERT_EQ(sparseSet.Capacity(), new_capacity);
	Log::Shutdown();
}


TEST(CoreSparseSet, CopySparseSet) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	using InstanceCounter = InstanceCount<int>;
	using AutoSparseSet = SparseSet<InstanceCounter, uint64_t>;
	using ScopeAutoSparseSet = ScopePtr<AutoSparseSet>;

	{
		AutoSparseSet rawSparseSet = AutoSparseSet();
		ASSERT_EQ(rawSparseSet.Count(), 0);
	}

	ScopeAutoSparseSet sparseSet = ScopeAutoSparseSet::Create(32);
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);

	uint32_t creator{0};
	const auto id1 = sparseSet->Create(creator++);
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 1);

	sparseSet->Create(creator++);

	sparseSet->Create(creator++);

	while (sparseSet->Count() < 32) {
		sparseSet->Create(creator++);
	}
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 32);

	sparseSet->Create(creator++);
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);

	{
		AutoSparseSet anotherSparseSet = *sparseSet;
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 66);
		for (int i = 0; i < 10; ++i) {
			anotherSparseSet.Remove(i);
		}
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 56);
	}
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);

	{
		AutoSparseSet anotherSparseSet(*sparseSet);
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 66);
		for (int i = 0; i < 10; ++i) {
			anotherSparseSet.Remove(i);
		}
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 56);
	}
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);

	sparseSet.Release();
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);
	Log::Shutdown();
}

TEST(CoreSparseSet, CopyAutoIdSparseSet) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});
	using InstanceCounter = InstanceCount<int>;
	using AutoSparseSet = AutoIdSparseSet<InstanceCounter, uint64_t>;
	using ScopeAutoSparseSet = ScopePtr<AutoSparseSet>;

	{
		AutoSparseSet rawSparseSet = AutoSparseSet();
		ASSERT_EQ(rawSparseSet.Count(), 0);
	}

	ScopeAutoSparseSet sparseSet = ScopeAutoSparseSet::Create(32);
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);

	const auto id1 = sparseSet->Create();
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 1);

	sparseSet->Create();

	sparseSet->Create();

	while (sparseSet->Count() < 32) {
		sparseSet->Create();
	}
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 32);

	sparseSet->Create(64);
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);

	{
		AutoSparseSet anotherSparseSet = *sparseSet;
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 66);
		for (int i = 0; i < 10; ++i) {
			anotherSparseSet.Remove(i);
		}
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 56);
	}
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);

	{
		AutoSparseSet anotherSparseSet(*sparseSet);
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 66);
		for (int i = 0; i < 10; ++i) {
			anotherSparseSet.Remove(i);
		}
		ASSERT_EQ(InstanceCounter::s_InstanceCount, 56);
	}
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);

	sparseSet.Release();
	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);
	Log::Shutdown();
}