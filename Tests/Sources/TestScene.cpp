//
// Created by ianpo on 15/05/2025.
//

#include "GlobalUsefullTests.hpp"

struct Physcs {
	Physcs() = default;
	~Physcs() = default;
	Physcs(const Physcs&) = default;
	Physcs& operator=(const Physcs&) = default;
	Vec3 vel{};
};
using PhyscsCounter = InstanceCount<Physcs>;

struct Render {
	Render() = default;
	~Render() = default;
	Render(const Render&) = default;
	Render& operator=(const Render&) = default;
	UUID meshId{};
};
using RenderCounter = InstanceCount<Render>;

TEST(CoreScene, RAII) {
	ScopePtr<Scene> scene = new Scene();

	ASSERT_EQ(scene->Count(), 0);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);
	scene->AddComponentType<PhyscsCounter>();
	scene->AddComponentType<RenderCounter>();

	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	const auto id = scene->CreateEntity();
	ASSERT_EQ(scene->Count(), 1);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	scene->AddComponent<PhyscsCounter>(id);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	scene->AddComponent<RenderCounter>(id);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 1);

	scene.Release();
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);
}