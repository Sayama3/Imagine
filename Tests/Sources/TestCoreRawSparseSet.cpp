//
// Created by ianpo on 15/05/2025.
//

#include "GlobalUsefullTests.hpp"

TEST(CoreRawSparseSet, IntRawSparseSet) {
	Log::Init();

	uint64_t idGenerator{0};
	{
		RawSparseSet rawSparseSet = RawSparseSet<>::Instantiate<int>();
		ASSERT_EQ(rawSparseSet.Count(), 0);
	}

	RawSparseSet rawSparseSet = RawSparseSet<>::Instantiate<int>(32);

	ASSERT_EQ(rawSparseSet.Count(), 0);
	ASSERT_EQ(rawSparseSet.Capacity(), 32);

	ASSERT_FALSE(rawSparseSet.Exist(0));
	ASSERT_FALSE(rawSparseSet.Exist(64));

	ASSERT_TRUE(rawSparseSet.Create(idGenerator++));
	ASSERT_FALSE(rawSparseSet.Create(0));
	ASSERT_TRUE(rawSparseSet.Exist(0));
	ASSERT_NE(rawSparseSet.Get(0), nullptr);
	ASSERT_EQ(rawSparseSet.Count(), 1);
	ASSERT_EQ(rawSparseSet.Capacity(), 32);

	int data1 = 1;
	ASSERT_TRUE(rawSparseSet.Create(idGenerator++, Buffer::Copy(data1)));
	ASSERT_FALSE(rawSparseSet.Create(1));
	ASSERT_TRUE(rawSparseSet.Exist(1));
	int* ptr1 = (int*)rawSparseSet.Get(1);
	ASSERT_EQ(*ptr1, data1);
	ASSERT_EQ(rawSparseSet.Count(), 2);
	ASSERT_EQ(rawSparseSet.Capacity(), 32);

	rawSparseSet.Remove(0);
	ASSERT_FALSE(rawSparseSet.Exist(0));
	ASSERT_TRUE(rawSparseSet.Exist(1));
	ptr1 = (int*)rawSparseSet.Get(1);
	ASSERT_EQ(*ptr1, 1);
	ASSERT_EQ(rawSparseSet.Count(), 1);
	ASSERT_EQ(rawSparseSet.Capacity(), 32);

	int data0 = -1;
	ASSERT_TRUE(rawSparseSet.Create(0, Buffer::Copy(data0)));
	ASSERT_FALSE(rawSparseSet.Create(0));
	ASSERT_TRUE(rawSparseSet.Exist(0));
	int* ptr0 = (int*)rawSparseSet.Get(0);
	ASSERT_EQ(*ptr0, data0);
	ASSERT_EQ(rawSparseSet.Count(), 2);
	ASSERT_EQ(rawSparseSet.Capacity(), 32);

	while (idGenerator < 32) {
		const int data = idGenerator;
		ASSERT_TRUE(rawSparseSet.Create(idGenerator++, Buffer::Copy(data)));
		ASSERT_FALSE(rawSparseSet.Create(idGenerator-1));
		ASSERT_TRUE(rawSparseSet.Exist(idGenerator-1));
		int* tmpPtr = (int*)rawSparseSet.Get(idGenerator-1);
		ASSERT_EQ(*tmpPtr, data);
		ASSERT_EQ(rawSparseSet.Count(), idGenerator);
		ASSERT_EQ(rawSparseSet.Capacity(), 32);
	}

	int lastData{127};
	ASSERT_TRUE(rawSparseSet.Create(128, Buffer::Copy(lastData)));
	ASSERT_FALSE(rawSparseSet.Create(128));
	ASSERT_TRUE(rawSparseSet.Exist(128));
	int* lastPtr = (int*)rawSparseSet.Get(128);
	ASSERT_EQ(*lastPtr, lastData);
	ASSERT_EQ(rawSparseSet.Count(), idGenerator + 1);
	ASSERT_GE(rawSparseSet.Capacity(), 32);

	const auto new_capacity = rawSparseSet.Capacity();

	rawSparseSet.Clear();

	ASSERT_EQ(rawSparseSet.Count(), 0);
	ASSERT_EQ(rawSparseSet.Capacity(), new_capacity);

	Log::Shutdown();
}

TEST(CoreRawSparseSet, BufferViewAndConstView) {

}

// TEST(CoreRawSparseSet, IntAutoRawSparseSet) {
// 	using AutoRawSparseSet = AutoIdRawSparseSet;
// 	{
// 		AutoRawSparseSet rawSparseSet{};
// 		ASSERT_EQ(rawSparseSet.Count(), 0);
// 	}
//
// 	AutoRawSparseSet rawSparseSet{32};
// 	ASSERT_EQ(rawSparseSet.Count(), 0);
// 	ASSERT_EQ(rawSparseSet.Capacity(), 32);
//
// 	ASSERT_FALSE(rawSparseSet.Exist(0));
// 	ASSERT_FALSE(rawSparseSet.Exist(64));
//
// 	const auto id1 = rawSparseSet.Create();
// 	ASSERT_NE(id1, (AutoRawSparseSet::NullId) );
// 	ASSERT_FALSE(rawSparseSet.Create(id1));
// 	ASSERT_TRUE(rawSparseSet.Exist(id1));
// 	ASSERT_EQ(rawSparseSet.Get(id1), Vec4());
// 	ASSERT_EQ(rawSparseSet.Count(), 1);
// 	ASSERT_EQ(rawSparseSet.Capacity(), 32);
//
// 	const auto id2 = rawSparseSet.Create(Vec4(1));
// 	ASSERT_FALSE(rawSparseSet.Create(id2));
// 	ASSERT_TRUE(rawSparseSet.Exist(id2));
// 	ASSERT_EQ(rawSparseSet.Get(id2), Vec4(1));
// 	ASSERT_EQ(rawSparseSet.Count(), 2);
// 	ASSERT_EQ(rawSparseSet.Capacity(), 32);
//
// 	rawSparseSet.Remove(id1);
// 	ASSERT_FALSE(rawSparseSet.Exist(id1));
// 	ASSERT_TRUE(rawSparseSet.Exist(id2));
// 	ASSERT_EQ(rawSparseSet.Get(id2), Vec4(1));
// 	ASSERT_EQ(rawSparseSet.Count(), 1);
// 	ASSERT_EQ(rawSparseSet.Capacity(), 32);
//
// 	ASSERT_TRUE(rawSparseSet.Create(id1, Vec4(-1)));
// 	ASSERT_FALSE(rawSparseSet.Create(id1));
// 	ASSERT_TRUE(rawSparseSet.Exist(id1));
// 	ASSERT_EQ(rawSparseSet.Get(id1), Vec4(-1));
// 	ASSERT_EQ(rawSparseSet.Count(), 2);
// 	ASSERT_EQ(rawSparseSet.Capacity(), 32);
//
// 	// It's available, the only one, so it should be filled.
// 	rawSparseSet.Remove(id1);
// 	ASSERT_EQ(rawSparseSet.Create(), id1);
//
// 	auto count = 2;
// 	while (rawSparseSet.Count() < 32) {
// 		const auto data = Vec4(rawSparseSet.Count()+1);
// 		const auto id = rawSparseSet.Create(data);
// 		ASSERT_NE(id, (AutoRawSparseSet::NullId));
// 		ASSERT_FALSE(rawSparseSet.Create(id));
// 		ASSERT_TRUE(rawSparseSet.Exist(id));
// 		ASSERT_EQ(rawSparseSet.Get(id), data);
// 		ASSERT_EQ(rawSparseSet.Count(), ++count);
// 		ASSERT_EQ(rawSparseSet.Capacity(), 32);
// 	}
//
// 	const auto id3 = 128;
// 	const auto data3 = Vec4(127);
// 	ASSERT_NE(rawSparseSet.Create(id3, data3), AutoRawSparseSet::NullId);
// 	ASSERT_FALSE(rawSparseSet.Create(128));
// 	ASSERT_TRUE(rawSparseSet.Exist(128));
// 	ASSERT_EQ(rawSparseSet.Get(128), data3);
// 	ASSERT_EQ(rawSparseSet.Count(), 33);
// 	ASSERT_GE(rawSparseSet.Capacity(), 32);
//
// 	const auto new_capacity = rawSparseSet.Capacity();
//
// 	rawSparseSet.Clear();
//
// 	ASSERT_EQ(rawSparseSet.Count(), 0);
// 	ASSERT_EQ(rawSparseSet.Capacity(), new_capacity);
// }
//
//
// TEST(CoreRawSparseSet, CounterRawSparseSet) {
// 	using InstanceCounter = InstanceCount<int>;
// 	using AutoRawSparseSet = AutoIdRawSparseSet<InstanceCounter, uint64_t>;
//
// 	AutoRawSparseSet rawSparseSet{32};
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);
//
// 	const auto id1 = rawSparseSet.Create();
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 1);
//
// 	rawSparseSet.Create();
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 2);
//
// 	rawSparseSet.Remove(id1);
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 1);
//
// 	rawSparseSet.Create();
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 2);
//
// 	while (rawSparseSet.Count() < 32) {
// 		rawSparseSet.Create();
// 	}
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 32);
//
// 	rawSparseSet.Create(64);
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 33);
//
// 	rawSparseSet.Clear();
//
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);
// 	for (int i = 0; i < 10; ++i) {
// 		rawSparseSet.Create();
// 	}
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 10);
//
// 	rawSparseSet.~AutoRawSparseSet();
// 	ASSERT_EQ(InstanceCounter::s_InstanceCount, 0);
// }
