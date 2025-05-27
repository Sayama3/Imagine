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

	Renderer * Renderer::Create(ApplicationParameters appParams) {
		Renderer* renderer{nullptr};
		if (!appParams.Renderer) {
			return nullptr;
		}

#if defined(MGN_RENDERER_VULKAN)
		renderer = new Vulkan::VulkanRenderer(appParams);
#else
		renderer = new CPU::CPURenderer(appParams);
#endif

		return renderer;
	}

	Renderer * Renderer::Initialize(ApplicationParameters appParams) {
		if (s_Renderer) return s_Renderer;
		s_Renderer = Renderer::Create(std::move(appParams));
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
