//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "ApplicationParameters.hpp"
#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Layers/LayerStack.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Window.hpp"

namespace Imagine::Core {

	class Application {
	public:
	private:
		static Application *s_Instance;

	public:
		static Application *Create(const ApplicationParameters &parameters);
		static Application *Initialize(const ApplicationParameters &parameters);
		static void Shutdown();
		static Application *Get();

	public:
		Application(const ApplicationParameters &parameters);
		~Application();

	public:
		void PushLayer(Layer *layer);
		void PushOverlay(Layer *overlay);

		template<typename T, typename... Args>
		void PushLayer(Args &&...args) { PushLayer(new T(std::forward<Args>(args)...)); }
		template<typename T, typename... Args>
		void PushOverlay(Args &&...args) { PushOverlay(new T(std::forward<Args>(args)...)); }

		template<typename T>
		inline T *FindLayer() {
			/*MGN_PROFILE_FUNCTION();*/
			return m_LayerStack.GetLayer<T>();
		}

		template<typename T>
		inline const T *FindLayer() const {
			/*MGN_PROFILE_FUNCTION();*/
			return m_LayerStack.GetLayer<T>();
		}

		template<typename T>
		inline bool TryGetLayer(T *&ptr) {
			/*MGN_PROFILE_FUNCTION();*/
			return m_LayerStack.TryGetLayer(ptr);
		}

		template<typename T>
		inline bool TryGetLayer(const T *&ptr) const {
			/*MGN_PROFILE_FUNCTION();*/
			return m_LayerStack.TryGetLayer(ptr);
		}

	public:
		void Stop();
		void Run();
		double Time() const;

	private:
		bool OnWindowClose(WindowCloseEvent &e);
		bool OnWindowResize(WindowResizeEvent &e);
	private:
		void OnEvent(Event& e);
	private:
		ApplicationParameters m_Parameters;

	private:
		Window *m_Window{nullptr};
		Renderer *m_Renderer{nullptr};

	private:
		LayerStack m_LayerStack;

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
		std::chrono::high_resolution_clock::time_point m_LastFrame;
		TimeStep m_DeltaTime;
		uint64_t m_CurrentFrame = 0;

	private:
		bool m_ShouldStop{false};
	};

} // namespace Imagine::Core
