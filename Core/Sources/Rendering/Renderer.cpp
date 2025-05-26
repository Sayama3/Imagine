//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Rendering/Renderer.hpp"

#if defined(MGN_RENDERER_VULKAN)
#include "Imagine/Vulkan/VulkanRenderer.hpp"
#else
#include "Imagine/CPU/CPURenderer.hpp"
#endif

namespace Imagine::Core {

	Renderer* Renderer::s_Renderer{nullptr};

	Renderer* Renderer::Initialize(const RendererParameters& renderParams, const ApplicationParameters& appParams) {
		if (s_Renderer) return s_Renderer;

#if defined(MGN_RENDERER_VULKAN)
		s_Renderer = new Vulkan::VulkanRenderer(renderParams, appParams);
#else
		s_Renderer = new CPU::CPURenderer(renderParams, appParams);
#endif

		return s_Renderer;
	}

	void Renderer::Shutdown()
	{
		delete s_Renderer;
		s_Renderer = nullptr;
	}

	Renderer* Renderer::Get()
	{
		return s_Renderer;
	}
}
