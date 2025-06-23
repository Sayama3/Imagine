//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "../Rendering/Renderer.hpp"
#include "ApplicationParameters.hpp"
#include "Window.hpp"

namespace Imagine::Core {

	class Application {
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
		void Stop();
		void Run();
		double Time() const;

	private:
		ApplicationParameters m_Parameters;

	private:
		Window *m_Window{nullptr};
		Renderer *m_Renderer{nullptr};

		EntityID m_CubeEntityID;

	private:
		std::chrono::high_resolution_clock::time_point m_Start;
		std::chrono::high_resolution_clock::time_point m_LastFrame;
		double m_DeltaTime;
		uint64_t m_CurrentFrame = 0;

	private:
		bool m_ShouldStop{false};
	};

} // namespace Imagine::Core
