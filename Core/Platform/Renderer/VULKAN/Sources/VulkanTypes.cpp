//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Vulkan/VulkanTypes.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Core;
namespace Imagine::Vulkan {
	MeshAsset::~MeshAsset(){
		VulkanRenderer* renderer = reinterpret_cast<VulkanRenderer*>(Renderer::Get());
		renderer->PushCurrentFrameDeletion(meshBuffers.vertexBuffer.allocation, meshBuffers.vertexBuffer.buffer);
		renderer->PushCurrentFrameDeletion(meshBuffers.indexBuffer.allocation, meshBuffers.indexBuffer.buffer);
	}
}