//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "ApplicationParameters.hpp"
#include "../Rendering/Renderer.hpp"
#include "Window.hpp"

namespace Imagine::Core {

	class Application {
	public:
		Application(const ApplicationParameters& parameters);
		~Application();
	public:
		void Stop();
		void Run();
		double Time() const;
	private:
		ApplicationParameters m_Parameters;
	private:
		Window* m_Window{nullptr};
		Renderer* m_Renderer{nullptr};
	private:
		std::chrono::high_resolution_clock::time_point m_Start;
		std::chrono::high_resolution_clock::time_point m_LastFrame;
		double m_DeltaTime;
		uint64_t m_CurrentFrame = 0;
	private:
		bool m_ShouldStop{false};
	};

} // Imagine
