//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Vulkan/VulkanTypes.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Core;
namespace Imagine::Vulkan {
	MeshAsset::~MeshAsset(){
		MGN_CORE_INFO("Request deletion of mesh asset '{}' for next frame.", this->name);
		VulkanRenderer* renderer = reinterpret_cast<VulkanRenderer*>(Renderer::Get());
		renderer->PushNextFrameDeletion(meshBuffers.vertexBuffer.allocation, meshBuffers.vertexBuffer.buffer);
		renderer->PushNextFrameDeletion(meshBuffers.indexBuffer.allocation, meshBuffers.indexBuffer.buffer);
	}
}