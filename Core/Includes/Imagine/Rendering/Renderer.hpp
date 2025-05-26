//
// Created by Sayama on 29/04/2025.
//

#pragma once
#include <Imagine/Application/ApplicationParameters.hpp>

namespace Imagine::Core {
#ifdef MGN_DEBUG
	inline static constexpr bool c_DefaultDebugRendering = true;
#else
	inline static constexpr bool c_DefaultDebugRendering = false;
#endif

	struct RendererParameters
	{
		uint16_t NbrFrameInFlight = 2;
		bool EnableDebug = c_DefaultDebugRendering;
	};

	class Renderer {
	public:
		static Renderer* Initialize(const RendererParameters& renderParams, const ApplicationParameters& appParams);
		static void Shutdown();
		static Renderer* Get();
	private:
		static Renderer* s_Renderer;
	public:
		Renderer() = default;
		virtual ~Renderer() {}
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
	public:
		virtual void Draw() = 0;
	};
}
