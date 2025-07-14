//
// Created by ianpo on 10/07/2025.
//

#include "Imagine/Layers/PhysicsLayer.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"


// Jolt includes
#include <Jolt/Core/Factory.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/RegisterTypes.h>

#include "Imagine/Components/Physicalisable.hpp"
#include "Imagine/Physics/PhysicsDebugRenderer.hpp"
#include "Imagine/Physics/PhysicsTypeHelpers.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Scene/SceneManager.hpp"
#include "Jolt/Physics/Collision/Shape/MeshShape.h"


namespace Imagine {

	PhysicsLayer::PhysicsLayer() {
	}

	PhysicsLayer::~PhysicsLayer() {
	}

	void PhysicsLayer::OnAttach() {
		// Create a factory, this class is responsible for creating instances of classes based on their name or hash and is mainly used for deserialization of saved data.
		// It is not directly used in this example but still required.
		JPH::Factory::sInstance = new JPH::Factory();

		// Register all physics types with the factory and install their collision handlers with the CollisionDispatch class.
		// If you have your own custom shape types you probably need to register their handlers with the CollisionDispatch before calling this function.
		// If you implement your own default material (PhysicsMaterial::sDefault) make sure to initialize it before this function or else this function will create one for you.
		JPH::RegisterTypes();

		m_PhysicsSystem = CreateScope<JPH::PhysicsSystem>();

		m_PhysicsSystem->Init(PhysicsLayer::cMaxBodies, PhysicsLayer::cNumBodyMutexes, PhysicsLayer::cMaxBodyPairs, PhysicsLayer::cMaxContactConstraints, m_BroadPhaseLayer, m_ObjectVsBroadphaseLayerFilter, m_ObjectVsObjectLayerFilter);

		// A body activation listener gets notified when bodies activate and go to sleep
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		m_PhysicsSystem->SetBodyActivationListener(&m_BodyActivationListener);

		// A contact listener gets notified when bodies (are about to) collide, and when they separate again.
		// Note that this is called from a job so whatever you do here needs to be thread safe.
		// Registering one is entirely optional.
		m_PhysicsSystem->SetContactListener(&m_ContactListener);

		// The main way to interact with the bodies in the physics system is through the body interface. There is a locking and a non-locking
		// variant of this. We're going to use the locking version (even though we're not planning to access bodies from multiple threads)
		m_BodyInterface = &m_PhysicsSystem->GetBodyInterface();

		// Draw Physics
		if (JPH::DebugRenderer::sInstance == nullptr) {
			JPH::DebugRenderer::sInstance = new PhysicsDebugRenderer();
		}
	}
	void PhysicsLayer::OnDetach() {

		// Draw Physics
		if (JPH::DebugRenderer::sInstance != nullptr) {
			delete JPH::DebugRenderer::sInstance;
			JPH::DebugRenderer::sInstance = nullptr;
		}

		m_BodyInterface = nullptr;
		m_PhysicsSystem.reset();

		// Unregisters all types with the factory and cleans up the default material
		JPH::UnregisterTypes();

		// Destroy the factory
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	void PhysicsLayer::OnEvent(Event &event) {
		EventDispatcher dispatch(event);
		dispatch.Dispatch<AppUpdateEvent>(MGN_DISPATCH_FALSE(OnUpdate));
		dispatch.Dispatch<ImGuiEvent>(MGN_DISPATCH_FALSE(OnImGui));
		dispatch.Dispatch<AppRenderEvent>(MGN_DISPATCH_FALSE(OnRender));
	}

	void PhysicsLayer::PushDeletion(JPH::BodyID body_id) {
		auto lock = std::scoped_lock(s_DeletionMutex);
		s_BodyToDelete.push_back(body_id);
	}

	bool PhysicsLayer::IsSimulating() {
		return s_Simulate;
	}

	void PhysicsLayer::OnUpdate(AppUpdateEvent &event) {
		MGN_PROFILE_FUNCTION();
		{
			MGN_PROFILE_SCOPE("Delete Some Bodies");
			auto lock = std::scoped_lock(s_DeletionMutex);
			m_BodyInterface->RemoveBodies(s_BodyToDelete.data(), static_cast<int>(s_BodyToDelete.size()));
			m_BodyInterface->DestroyBodies(s_BodyToDelete.data(), static_cast<int>(s_BodyToDelete.size()));
			s_BodyToDelete.clear();
		}

		if (s_Simulate) {
			MGN_PROFILE_SCOPE("Physics Simulation");
			const int cCollisionSteps = 4;
			m_PhysicsSystem->Update(event.GetTimeStep().GetSeconds(), cCollisionSteps, &m_TempAllocator, &m_JobSystem);
		}

		{
			MGN_PROFILE_SCOPE("Update All Scenes");
			for (auto scene: SceneManager::GetLoadedScenes()) {
				Update(scene.get(), event.GetTimeStep());
			}
		}
	}
	void PhysicsLayer::OnRender(AppRenderEvent &event) {
		if (!JPH::DebugRenderer::sInstance) return;
		// TODO: Fetch the 'JPH::BodyManager::DrawSettings' from some project settings somewhere
		static JPH::BodyManager::DrawSettings s_DrawSettings{
				false, // mDrawGetSupportFunction
				false, // mDrawSupportDirection
				false, // mDrawGetSupportingFace
				false, // mDrawShape
				false, // mDrawShapeWireframe
				JPH::BodyManager::EShapeColor::MotionTypeColor, // mDrawShapeColor
				true, // mDrawBoundingBox
				false, // mDrawCenterOfMassTransform
				false, // mDrawWorldTransform
				false, // mDrawVelocity
				false, // mDrawMassAndInertia
				false, // mDrawSleepStats
				false, // mDrawSoftBodyVertices
				false, // mDrawSoftBodyVertexVelocities
				false, // mDrawSoftBodyEdgeConstraints
				false, // mDrawSoftBodyBendConstraints
				false, // mDrawSoftBodyVolumeConstraints
				false, // mDrawSoftBodySkinConstraints
				false, // mDrawSoftBodyLRAConstraints
				false, // mDrawSoftBodyPredictedBounds
				JPH::ESoftBodyConstraintColor::ConstraintType, // mDrawSoftBodyConstraintColor
		};

		((PhysicsDebugRenderer *) JPH::DebugRenderer::sInstance)->SetCameraPos(Convert(Camera::s_MainCamera->position));
		m_PhysicsSystem->DrawBodies(s_DrawSettings, JPH::DebugRenderer::sInstance);
	}

	void PhysicsLayer::OnImGui(ImGuiEvent &event) {
#ifdef MGN_IMGUI
		ImGui::SetNextWindowSize({200, 200}, ImGuiCond_FirstUseEver);
		ImGui::Begin("Physics");
		{
			if (ImGui::Checkbox("Simulate", &s_Simulate) && s_Simulate) {
				m_PhysicsSystem->OptimizeBroadPhase();
			}
		}
		ImGui::End();
#endif
	}

	void PhysicsLayer::Update(Scene *scene, TimeStep ts) {

		if (!s_Simulate) {
			MGN_PROFILE_SCOPE("Update Physics Components");
			scene->ForEachWithComponent<Physicalisable>([this](Scene *scene, EntityID id, Physicalisable &comp) {
				const TransformR trs = scene->GetTransform(id);
				const Vec3 pos = trs.LocalPosition;
				const Quat rot = trs.LocalRotation;
				const Vec3 lin = comp.GetLinearVelocity();
				const Vec3 ang = comp.GetRadAngularVelocity();
				const JPH::EActivation activation = comp.GetActivation();

				// if (!scene->BeginRelationship(id).IsRoot()) {
				// 	scene->MoveToRoot(id);
				// 	Entity& e = scene->GetEntity(id);
				// 	e.LocalPosition = trs.GetWorldPosition();
				// 	e.LocalRotation = trs.GetWorldRotation();
				// 	e.LocalScale = trs.GetWorldScale();
				// }

				if (comp.BodyID.IsInvalid()) {
					const JPH::Shape *shp = comp.GetShape();
					if (!shp) return;
					JPH::BodyCreationSettings creationSettings = {shp, JPH::RVec3Arg{pos.x, pos.y, pos.z}, JPH::QuatArg{rot.x, rot.y, rot.z, rot.w}, comp.GetMotionType(), comp.GetLayer()};
					creationSettings.mAllowDynamicOrKinematic = true;
					comp.BodyID = m_BodyInterface->CreateAndAddBody(creationSettings, activation);
					m_BodyInterface->SetFriction(comp.BodyID, comp.GetFriction());
					m_BodyInterface->SetGravityFactor(comp.BodyID, comp.GetGravityFactor());
				}
				else {
					m_BodyInterface->SetPositionAndRotation(comp.BodyID, JPH::RVec3Arg{pos.x, pos.y, pos.z}, JPH::QuatArg{rot.x, rot.y, rot.z, rot.w}, comp.GetActivation());
					if (comp.dirty) {
						const JPH::Shape *shp = comp.GetShape();
						m_BodyInterface->SetShape(comp.BodyID, shp, true, activation);
						m_BodyInterface->SetMotionType(comp.BodyID, comp.GetMotionType(), activation);
						m_BodyInterface->SetObjectLayer(comp.BodyID, comp.GetLayer());
						m_BodyInterface->SetFriction(comp.BodyID, comp.GetFriction());
						m_BodyInterface->SetGravityFactor(comp.BodyID, comp.GetGravityFactor());
					}
				}
				comp.dirty = false;
			});
		}
		else {
			MGN_PROFILE_SCOPE("Update Scene Transform");
			scene->ForEachWithComponent<Physicalisable>([this](Scene *scene, EntityID id, Physicalisable &comp) {
				if (comp.BodyID.IsInvalid()) return;
				const TransformR trs = scene->GetTransform(id);
				const auto wpos = Convert(m_BodyInterface->GetPosition(comp.BodyID));
				const auto wrot = Convert(m_BodyInterface->GetRotation(comp.BodyID));
				Entity &entity = scene->GetEntity(id);
				entity.LocalPosition = wpos;
				entity.LocalRotation = wrot;
				const Vec3 lin = comp.GetLinearVelocity();
				const Vec3 ang = comp.GetRadAngularVelocity();
				m_BodyInterface->AddLinearAndAngularVelocity(comp.BodyID, JPH::Vec3(lin.x, lin.y, lin.z), JPH::Vec3(ang.x, ang.y, ang.z));

				comp.SetLinearVelocity(Vec3(0));
				comp.SetRadAngularVelocity(Vec3(0));
			});
			scene->CacheTransforms();
		}
	}
} // namespace Imagine
