//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Vulkan/VulkanTypes.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine::Core;
namespace Imagine::Vulkan {
	MeshAsset::~MeshAsset() {
		VulkanRenderer *renderer = reinterpret_cast<VulkanRenderer *>(Renderer::Get());
		renderer->PushNextFrameDeletion(meshBuffers.vertexBuffer.allocation, meshBuffers.vertexBuffer.buffer);
		renderer->PushNextFrameDeletion(meshBuffers.indexBuffer.allocation, meshBuffers.indexBuffer.buffer);
	}
	ManualDeleteMeshAsset::ManualDeleteMeshAsset(ManualDeleteMeshAsset && o) noexcept : Mesh(std::move(o)), meshBuffers(o.meshBuffers){
	}
	ManualDeleteMeshAsset &ManualDeleteMeshAsset::operator=(ManualDeleteMeshAsset && o) noexcept {
		swap(o);
		return *this;
	}
	void ManualDeleteMeshAsset::swap(ManualDeleteMeshAsset &other) noexcept {
		Mesh::swap(other);
		std::swap(meshBuffers, other.meshBuffers);
	}
} // namespace Imagine::Vulkan
