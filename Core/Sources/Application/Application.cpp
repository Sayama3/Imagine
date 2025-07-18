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
#include "Imagine/Physics/PhysicsDebugRenderer.hpp"
#include "Imagine/Rendering/CPU/CPUModel.hpp"

#ifdef MGN_IMGUI
#include "Imagine/ThirdParty/ImGui.hpp"
#endif
namespace Imagine {

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

		SceneManager::Intialize();

		Project::New();

		m_ShouldStop = false;
		m_LastFrame = m_Start = std::chrono::high_resolution_clock::now();
		m_DeltaTime = 0.01666666f;

		if (parameters.Window) {
			m_Window = Window::Initialize(parameters.AppName, parameters.Window.value());
			m_Window->SetEventCallback(MGN_BIND_EVENT_FN(Application::OnEvent));
		}

		if (parameters.Renderer) {
			Project::AddOnLoad(CPUShader::TryRegister);
			Project::AddOnLoad(CPUMaterial::TryRegister);

			auto vert = CPUFileShader::Initialize(ShaderStage::Vertex, Path{FileSource::Engine, "pbr.vert.spv"});
			auto frag = CPUFileShader::Initialize(ShaderStage::Fragment, Path{FileSource::Engine, "pbr.frag.spv"});
			CPUShader::TryRegister();

			CPUMaterial::InitDefaultMaterials(vert->Handle, frag->Handle);
			CPUMaterial::TryRegister();

			m_Renderer = Renderer::Initialize(parameters);
		}
#ifdef MGN_IMGUI
		ThirdParty::ImGuiLib::CreateContext();
		ThirdParty::ImGuiLib::InitializeWindow();
		ThirdParty::ImGuiLib::InitializeRenderer();

		ThirdParty::ImGuiLib::EnableDocking();
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
		ThirdParty::ImGuiLib::ShutdownRenderer();
		ThirdParty::ImGuiLib::ShutdownWindow();
		ThirdParty::ImGuiLib::Shutdown();
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
		{
			auto model = CPUModel::LoadModel({FileSource::Engine, "Models/Sponza/Sponza.gltf"});
			Project::GetActive()->GetFileAssetManager()->AddAsset(model, {FileSource::Engine, "Models/Sponza/Sponza.gltf"});
			const auto entityId = SceneManager::GetMainScene()->CreateEntity();
			Renderable *renderable = SceneManager::GetMainScene()->AddComponent<Renderable>(entityId);
			renderable->cpuMeshOrModel = model->Handle;
			MGN_CORE_CASSERT(model);
		}


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
		ThirdParty::ImGuiLib::NewRenderFrame();
		ThirdParty::ImGuiLib::NewWindowFrame();
		ThirdParty::ImGuiLib::NewFrame();

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
			ThirdParty::ImGuiLib::Render();
		}
#endif
	}

	void Application::DrawScenes() {
		MGN_PROFILE_FUNCTION();

		GPUSceneData sceneData{};
		GPULightData lightData{};

		std::atomic<int> counter{0};

		for (auto &scene: SceneManager::GetLoadedScenes()) {
			scene->ForEachWithComponent<Light>([this, &lightData, &counter](const Scene *scn, const EntityID id, const Light &comp) {
				const auto index = counter.fetch_add(1, std::memory_order_relaxed);
				if (index >= GPULightData::MaxLight) return;
				Light light = comp;
				const auto world = scn->GetWorldTransform(id);
				const auto normal = glm::transpose(glm::inverse(world));
				light.position += scn->GetWorldTransform(id) * Vec4(scn->GetEntity(id).LocalPosition, 1);
				const auto w = light.direction.w;
				const auto len = Math::Magnitude(glm::fvec3(light.direction));
				light.direction = normal * glm::fvec4(0, -1, 0, 0);
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
					ctx.OpaqueLines.reserve(scene->Count()*3);
					scene->ForEach([&ctx](const Scene *scene, const EntityID id) {
						const auto trs = scene->GetTransform(id);
						const auto pos = trs.GetWorldPosition();
						ctx.OpaqueLines.push_back({{Vertex::PC(pos, Vec4(1,0,0,1)), Vertex::PC(pos + trs.GetRight(), Vec4(1,0,0,1))}});
						ctx.OpaqueLines.push_back({{Vertex::PC(pos, Vec4(0,1,0,1)), Vertex::PC(pos + trs.GetUp(), Vec4(0,1,0,1))}});
						ctx.OpaqueLines.push_back({{Vertex::PC(pos, Vec4(0,0,1,1)), Vertex::PC(pos + trs.GetForward(), Vec4(0,0,1,1))}});
					});
					scene->ForEachWithComponent<Renderable>([&ctx](const Scene *scene, const EntityID id, Renderable &renderable) {
						if (renderable.cpuMeshOrModel == NULL_ASSET_HANDLE) return;
						Ref<Asset> asset = AssetManager::GetAsset(renderable.cpuMeshOrModel);
						if (!asset) return;
						const Mat4 worldMat = scene->GetWorldTransform(id);
						switch (asset->GetType()) {
							case AssetType::Model: {
								auto cpuModel = CastPtr<CPUModel>(asset);
								if (!cpuModel) return;
								if (cpuModel->LoadModelInGPU())
								for (const CPUModel::Node& node: cpuModel->Nodes) {
									if (node.meshes.empty()) continue;
									const auto world = worldMat * node.worldMatrix;
									for (const Weak<CPUMesh> & mesh: node.meshes) {
										if (const Ref<CPUMesh> lock = mesh.lock()) {
											if (lock->gpu) ctx.OpaqueSurfaces.emplace_back(world, lock->gpu);
										}
									}
								}
							}
								break;
							case AssetType::Mesh: {
								auto cpuMesh = CastPtr<CPUMesh>(asset);
								cpuMesh->LoadMeshInGPU();
								auto gpuMesh = cpuMesh->gpu;
								if (!gpuMesh) return;
								MGN_CORE_MASSERT(worldMat != Mat4(0), "The transform wasn't cached for the entity '{}'", scene->GetName(id));
								ctx.OpaqueSurfaces.emplace_back(worldMat, gpuMesh);
							} break;
							default:
								return;
						}
					});
					m_Renderer->Draw(ctx);
					ctx.Clear();
				}
			}

			{
				MGN_PROFILE_SCOPE("Physics Rendering");
				DrawContext ctx;
				if (!PhysicsDebugRenderer::s_Lines.empty()) {
					ctx.OpaqueLines = std::move(PhysicsDebugRenderer::s_Lines);
				}
				if (!PhysicsDebugRenderer::s_Vertices.empty()) {
					Scope<CPUMesh> mesh = CreateScope<CPUMesh>(std::move(PhysicsDebugRenderer::s_Vertices));
					mesh->Lods.emplace_back(0, (uint32_t) mesh->Indices.size(), NULL_ASSET_HANDLE);
					mesh->gpu = m_Renderer->LoadMesh(*mesh);
					ctx.OpaqueSurfaces.emplace_back(Math::Identity<Mat4>(), mesh->gpu);
				}
				m_Renderer->Draw(ctx);
			}

			m_Renderer->EndDraw();

#ifdef MGN_IMGUI
			ThirdParty::ImGuiLib::PostRender();
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
} // namespace Imagine
