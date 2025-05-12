//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Application/Application.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Scene/Renderable.hpp"
#include "Imagine/Scene/Scene.hpp"


namespace Imagine::Core {

	class TmpElement {
	public:
		inline static uint64_t ElementsCount{0};
		TmpElement() {
			ElementsCount += 1;
			MGN_LOG_DEBUG("Create Tmp Element. Element Count = {}", ElementsCount);
		}
		~TmpElement() {
			ElementsCount -= 1;
			MGN_LOG_DEBUG("Delete Tmp Element. Element Count = {}", ElementsCount);
		}
	};

	Application::Application(const ApplicationParameters& parameters) : m_Parameters(parameters) {
		m_ShouldStop = false;
		m_LastFrame = m_Start = std::chrono::high_resolution_clock::now();
		m_DeltaTime = 0.01666666f;

		if (parameters.Window) {
			m_Window = Window::Create(parameters.Name, parameters.Window.value());
		}

		if (parameters.UseRenderer) {
			//TODO: Create the renderer.
		}
	}

	Application::~Application() {
	}

	void Application::Stop() {
		m_ShouldStop = true;
	}

	void Application::Run() {
		// Scene* scene = new Scene();

		// UUID entityId = scene->CreateEntity().ID;
		// BufferView view = scene->AddComponent<Renderable>(entityId);
		// view.Get<Renderable>().m_Mesh = UUID::Null();
		// view.Get<Renderable>().m_Material = UUID::Null();

		MGN_CORE_TRACE("ElementsCount - {}", TmpElement::ElementsCount);
		auto* set = new AutoIdSparseSet<TmpElement>();
		uint32_t id = set->Create();
		for (int i = 0; i < 32; ++i) {
			set->Create();
		}
		set->Remove(3);
		set->Remove(6);
		set->Remove(7);
		set->Remove(8);
		set->Create();
		set->Create();
		set->Remove(9);
		set->Remove(10);
		set->Remove(id);
		while (!m_ShouldStop) {
			if (m_Window) {
				m_Window->Update();
				m_ShouldStop |= m_Window->ShouldClose();
			}
			std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
			m_DeltaTime = std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_LastFrame).count();
			m_LastFrame = newFrame;
			MGN_CORE_INFO("Frame #{}", m_CurrentFrame);
			MGN_CORE_INFO("DeltaTime #{}", m_DeltaTime);
			MGN_CORE_INFO("Time #{}", Time());

			// MGN_CORE_INFO("Entity {} at position {}", entityId.string(), Math::ToString(scene->GetEntity(entityId).Position)*+£);

			// scene->GetEntity(entityId).Position += Vec3{m_DeltaTime, 0, 0};

			m_CurrentFrame += 1;
		}
		// delete scene;
		MGN_CORE_TRACE("ElementsCount - {}", TmpElement::ElementsCount);
		delete set;
		MGN_CORE_TRACE("ElementsCount - {}", TmpElement::ElementsCount);
	}

	double Application::Time() const {
		std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_Start).count();
	}
} // Imagine