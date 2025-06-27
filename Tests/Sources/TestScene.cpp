//
// Created by ianpo on 15/05/2025.
//

#include "GlobalUsefullTests.hpp"

struct Physcs {
	Physcs() = default;
	Physcs(const Vec3 &vel) :
		vel(vel) {};
	~Physcs() = default;
	Physcs(const Physcs &other) = default;
	Physcs &operator=(const Physcs &) = default;
	Vec3 vel{};
};
using PhyscsCounter = InstanceCount<Physcs>;

struct Render {
	Render() = default;
	~Render() = default;
	Render(const Render &) = default;
	Render &operator=(const Render &) = default;
	Core::UUID meshId{};
};
using RenderCounter = InstanceCount<Render>;

TEST(CoreScene, RAII) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});

	ScopePtr<Scene> scene = new Scene();

	ASSERT_EQ(scene->Count(), 0);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);
	scene->RegisterType<PhyscsCounter>();
	scene->RegisterType<RenderCounter>();

	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	const auto id = scene->CreateEntity();
	ASSERT_EQ(scene->Count(), 1);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	EXPECT_TRUE(scene->AddComponent<PhyscsCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);
	EXPECT_FALSE(scene->AddComponent<PhyscsCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);
	EXPECT_TRUE(scene->GetComponent<PhyscsCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);
	EXPECT_TRUE(scene->GetOrAddComponent<PhyscsCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	EXPECT_TRUE(scene->AddComponent<RenderCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 1);
	EXPECT_FALSE(scene->AddComponent<RenderCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 1);
	EXPECT_TRUE(scene->GetComponent<RenderCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 1);
	EXPECT_TRUE(scene->GetOrAddComponent<RenderCounter>(id));
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 1);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 1);

	scene->Clear();
	ASSERT_EQ(scene->Count(), 0);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	EntityID ids[50];
	for (int i = 0; i < 50; ++i) {
		ids[i] = scene->CreateEntity();
	}
	ASSERT_EQ(scene->Count(), 50);
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	for (int i = 0; i < 20; ++i) {
		scene->AddComponent(ids[i], Core::UUID::FromType<PhyscsCounter>());
	}
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 20);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	for (int i = 20; i < 40; ++i) {
		scene->AddComponent(ids[i], Core::UUID::FromType<RenderCounter>());
	}
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 20);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 20);

	for (int i = 40; i < 50; ++i) {
		scene->AddComponent(ids[i], Core::UUID::FromType<PhyscsCounter>());
		scene->AddComponent(ids[i], Core::UUID::FromType<RenderCounter>());
	}
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 30);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 30);

	for (int i = 15; i < 25; ++i) {
		scene->GetOrAddComponent(ids[i], Core::UUID::FromType<PhyscsCounter>());
		scene->GetOrAddComponent(ids[i], Core::UUID::FromType<RenderCounter>());
	}
	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 35);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 35);

	scene.Release();

	ASSERT_EQ(PhyscsCounter::s_InstanceCount, 0);
	ASSERT_EQ(RenderCounter::s_InstanceCount, 0);

	Log::Shutdown();
}


TEST(CoreScene, Persistence) {
	Log::Init({std::nullopt, c_DefaultLogPattern, true});

	Scene scene;

	scene.RegisterType<Physcs>();
	scene.Prepare(50);

	EntityID ids[50];

	for (int i = 0; i < 50; ++i) {
		ids[i] = scene.CreateEntity();
		scene.AddComponent<Physcs>(ids[i], Vec3(static_cast<Real>(i)));
	}

	for (int i = 0; i < 50; ++i) {
		const Physcs *comp = scene.GetComponent<Physcs>(ids[i]);
		const Vec3 reference = Vec3{static_cast<Real>(i)};
		ASSERT_TRUE(comp != nullptr);
		ASSERT_EQ(reference, comp->vel);
	}

	for (int i = 0; i < 50; ++i) {
		auto *comp = scene.GetComponent<Physcs>(ids[i]);
		comp->vel = Vec3{static_cast<Real>(50 - i)};
	}

	for (int i = 0; i < 50; ++i) {
		const Physcs *comp = scene.GetComponent<Physcs>(ids[i]);
		const Vec3 reference = Vec3{static_cast<Real>(50 - i)};
		ASSERT_TRUE(comp != nullptr);
		ASSERT_EQ(reference, comp->vel);
	}

	Log::Shutdown();
}
