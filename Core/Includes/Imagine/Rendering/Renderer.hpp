//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "Imagine/Application/ApplicationParameters.hpp"
#include "Imagine/Rendering/RendererParameters.hpp"

namespace Imagine::Core {

	class Renderer {
	public:
		static Renderer* Create(ApplicationParameters appParams);
		static Renderer* Initialize(ApplicationParameters appParams);
		static void Shutdown();
		static Renderer* Get();
	private:
		static Renderer* s_Renderer;
	public:
		Renderer() = default;
		virtual ~Renderer() = default;
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
	public:
		virtual void Draw() = 0;
		virtual void PrepareShutdown() = 0;
	};
}
