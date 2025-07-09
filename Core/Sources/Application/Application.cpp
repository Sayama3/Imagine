//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Application/Application.hpp"

#include "Imagine/Assets/AssetManager.hpp"
#include "Imagine/Components/Renderable.hpp"
#include "Imagine/Core/Inputs.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Profiling.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/Scene/SceneManager.hpp"

#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Events/KeyEvent.hpp"
#include "Imagine/Events/MouseEvent.hpp"
#include "Imagine/Rendering/CPU/CPUModel.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include "Imagine/Rendering/MgnImGui.hpp"
#endif
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


	Application *Application::s_Instance{nullptr};

	Application *Application::Create(const ApplicationParameters &parameters) {
		Application *application{nullptr};
		application = new Application(parameters);
		return application;
	}

	Application *Application::Initialize(const ApplicationParameters &parameters) {
		if (s_Instance) return s_Instance;
		s_Instance = Application::Create(parameters);
		return s_Instance;
	}

	void Application::Shutdown() {
		delete s_Instance;
		s_Instance = nullptr;
	}

	Application *Application::Get() {
		return s_Instance;
	}

	Application::Application(const ApplicationParameters &parameters) :
		m_Parameters(parameters) {

		m_ShouldStop = false;
		m_LastFrame = m_Start = std::chrono::high_resolution_clock::now();
		m_DeltaTime = 0.01666666f;

		if (parameters.Window) {
			m_Window = Window::Initialize(parameters.AppName, parameters.Window.value());
			m_Window->SetEventCallback(MGN_BIND_EVENT_FN(Application::OnEvent));
		}

		if (parameters.Renderer) {
			Ref<CPUShader> vert = CreateRef<CPUFileShader>(ShaderStage::Vertex, Path{FileSource::Assets, "pbr.vert.spv"});
			Ref<CPUShader> frag = CreateRef<CPUFileShader>(ShaderStage::Fragment, Path{FileSource::Assets, "pbr.frag.spv"});

			Project::GetActive()->GetAssetManager()->AddAsset(vert);
			Project::GetActive()->GetAssetManager()->AddAsset(frag);

			CPUMaterial::InitDefaultMaterials(vert->Handle, frag->Handle);

			m_Renderer = Renderer::Initialize(parameters);
		}
#ifdef MGN_IMGUI
		MgnImGui::CreateContext();
		MgnImGui::InitializeWindow();
		MgnImGui::InitializeRenderer();

		MgnImGui::EnableDocking();
#endif
	}

	Application::~Application() {
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			(*it)->OnDetach();
		}
		m_LayerStack.Clear();

		SceneManager::Shutdown();

		Project::Shutdown();

		if (m_Renderer) {
			m_Renderer->PrepareShutdown();
		}

		CPUMaterial::DestroyDefaultMaterials();

#ifdef MGN_IMGUI
		MgnImGui::ShutdownRenderer();
		MgnImGui::ShutdownWindow();
		MgnImGui::Shutdown();
#endif

		if (m_Renderer) {
			m_Renderer = nullptr;
			Renderer::Shutdown();
		}

		if (m_Window) {
			m_Window = nullptr;
			Window::Shutdown();
		}
	}

	void Application::PushLayer(Layer *layer) {
		MGN_PROFILE_FUNCTION();
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer *overlay) {
		MGN_PROFILE_FUNCTION();
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Stop() {
		m_ShouldStop = true;
	}

	void Application::Run() {
		auto model = CPUModel::LoadModel("C:/Users/ianpo/Documents/GitHub/glTF-Sample-Assets/Models/Sponza/glTF/Sponza.gltf", SceneManager::GetMainScene().get());

		MGN_CORE_CASSERT(model);

		while (!m_ShouldStop) {
			MGN_FRAME_START();
			bool canDraw = true;

			if (m_Window) {
				MGN_PROFILE_SCOPE("Windows Update");
				m_Window->Update();
				canDraw = !m_Window->IsMinimized();
			}

			{
				MGN_PROFILE_SCOPE("Event - App Tick");
				AppTickEvent event{m_DeltaTime};
				for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
					(*it)->OnEvent(event);
					if (event.m_Handled) {
						break;
					}
				}
			}

			{
				MGN_PROFILE_SCOPE("Event - App Update");
				AppUpdateEvent event{m_DeltaTime};
				for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
					(*it)->OnEvent(event);
					if (event.m_Handled) {
						break;
					}
				}
			}


			if (canDraw) {
				MGN_PROFILE_SCOPE("App Draw");
				Camera::s_MainCamera->Update(m_DeltaTime);
				if (model) {
					for (auto & tex: model->Textures) {
						tex->gpu = Renderer::Get()->LoadTexture2D(*tex);
					}
					for (Ref<CPUMaterial> & material: model->Materials) {
						material->gpu = Renderer::Get()->LoadMaterial(*material);
					}
					for (auto & instance: model->Instances) {
						instance->gpu = Renderer::Get()->LoadMaterialInstance(*instance);
					}
					for (auto & mesh: model->Meshes) {
						mesh->gpu = Renderer::Get()->LoadMesh(*mesh);
					}
					model.reset();
				}
				Draw();
			}

			{
				MGN_PROFILE_SCOPE("Delta Time Update");
				std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
				m_DeltaTime = std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_LastFrame).count();
				m_LastFrame = newFrame;
				m_CurrentFrame += 1;
			}

			// MGN_CORE_INFO("Frame #{}", m_CurrentFrame);
			// MGN_CORE_INFO("DeltaTime #{}", m_DeltaTime);
			// MGN_CORE_INFO("Time #{}", Time());
			MGN_FRAME_END();
		}
	}

	double Application::Time() const {
		std::chrono::high_resolution_clock::time_point newFrame = std::chrono::high_resolution_clock::now();
		return std::chrono::duration<double, std::chrono::seconds::period>(newFrame - m_Start).count();
	}

	void Application::OnEvent(Event &e, bool fromNormalEventQueue) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>([this, fromNormalEventQueue](WindowResizeEvent &event) { return this->OnWindowResize(event, !fromNormalEventQueue); });

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			(*it)->OnEvent(e);
			if (e.m_Handled) {
				break;
			}
		}
	}
	void Application::Draw() {
		MGN_PROFILE_FUNCTION();

		DrawImGui();

		DrawScenes();
	}

	void Application::DrawImGui() {
		MGN_PROFILE_FUNCTION();
#ifdef MGN_IMGUI
		MgnImGui::NewRenderFrame();
		MgnImGui::NewWindowFrame();
		MgnImGui::NewFrame();

		{

			MGN_PROFILE_SCOPE("Event ImGui");
			ImGuiEvent event(m_DeltaTime);
			for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
				(*it)->OnEvent(event);
				if (event.m_Handled) {
					break;
				}
			}
		}

		if (m_Window) {
			MGN_PROFILE_SCOPE("ImGui Window");
			m_Window->SendImGuiCommands();
		}

		if (m_Renderer) {
			MGN_PROFILE_SCOPE("ImGui Renderer");
			m_Renderer->SendImGuiCommands();
		}

		{
			MGN_PROFILE_SCOPE("ImGui Main Scene");
			SceneManager::GetMainScene()->SendImGuiCommands();
		}

		// TODO: Other imgui rendering functions
		{
			MGN_PROFILE_SCOPE("ImGui Render");
			MgnImGui::Render();
		}
#endif
	}

	void Application::DrawScenes() {
		MGN_PROFILE_FUNCTION();

		GPUSceneData sceneData{};
		GPULightData lightData{};

		std::atomic<int> counter{0};

		for (auto& scene : SceneManager::GetLoadedScenes()) {
			scene->ForEachWithComponent<Light>([this,&lightData,&counter](const Scene* scn, const EntityID id, const Light& comp) {
				const auto index = counter.fetch_add(1, std::memory_order_relaxed);
				if (index >= GPULightData::MaxLight) return;
				Light light = comp;
				const auto world = scn->GetWorldTransform(id);
				const auto normal = glm::transpose(glm::inverse(world));
				light.position += scn->GetWorldTransform(id) * Vec4(scn->GetEntity(id).LocalPosition, 1);
				const auto w = light.direction.w;
				const auto len = Math::Magnitude(glm::fvec3(light.direction));
				light.direction = normal * glm::fvec4(0,-1,0,0);
				light.direction *= len;
				light.direction.w = w;
				lightData.lights[index] = light;
			});
		}

		lightData.lightCount = std::min(counter.load(std::memory_order_acquire), GPULightData::MaxLight);

		if (m_Renderer->BeginDraw(sceneData, lightData)) {
			m_Renderer->Draw();


			// TODO: See if I wanna do it this way
			{
				MGN_PROFILE_SCOPE("Event Rendering");
				AppRenderEvent event{m_DeltaTime};
				for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
					(*it)->OnEvent(event);
					if (event.m_Handled) {
						break;
					}
				}
			}

			{
				MGN_PROFILE_SCOPE("Draw All Scenes");

				auto loadedScene = SceneManager::GetLoadedScenes();
				DrawContext ctx{};
				for (const std::shared_ptr<Scene> &scene: loadedScene) {
					MGN_PROFILE_SCOPE("Draw One Scene");
					scene->CacheTransforms();
					ctx.OpaqueSurfaces.reserve(scene->CountComponents<Renderable>());
					scene->ForEachWithComponent<Renderable>([&ctx](const Scene *scene, const EntityID id, Renderable &renderable) {
						auto gpuMesh = AssetManager::GetAssetAs<CPUMesh>(renderable.cpuMesh)->gpu;
						if (!gpuMesh) return;

						const Mat4 worldMat = scene->GetWorldTransform(id);
						MGN_CORE_MASSERT(worldMat != Mat4(0), "The transform wasn't cached for the entity '{}'", scene->GetName(id));
						ctx.OpaqueSurfaces.emplace_back(worldMat, gpuMesh);
					});
					m_Renderer->Draw(ctx);
					ctx.Clear();
				}
			}

			m_Renderer->EndDraw();

#ifdef MGN_IMGUI
			MgnImGui::PostRender();
#endif

			m_Renderer->Present();
		}
	}


	bool Application::OnWindowClose(WindowCloseEvent &e) {
		m_ShouldStop = true;
		return false;
	}

	bool Application::OnWindowResize(WindowResizeEvent &e, bool shouldRedraw) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			return false;
		}

		const bool canRedraw = m_Renderer->Resize();

		if (canRedraw && shouldRedraw) {
			Draw();
		}

		return false;
	}
} // namespace Imagine::Core
