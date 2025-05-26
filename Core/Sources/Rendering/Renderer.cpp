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
	Renderer* Renderer::CreateRenderer(const RendererParameter& renderParams, const ApplicationParameters& appParams) {
#if defined(MGN_RENDERER_VULKAN)
		return new Vulkan::VulkanRenderer(renderParams, appParams);
#else
		return new CPU::CPURenderer(renderParams, appParams);
#endif
	}
}
