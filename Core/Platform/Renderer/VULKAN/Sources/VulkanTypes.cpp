//
// Created by Iannis on 23/06/2025.
//

#include "Imagine/Vulkan/VulkanTypes.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"

using namespace Imagine;

namespace Imagine::Vulkan {
	AutoDeleteMeshAsset::AutoDeleteMeshAsset() = default;
	AutoDeleteMeshAsset::~AutoDeleteMeshAsset() {
		VulkanRenderer *renderer = reinterpret_cast<VulkanRenderer *>(Renderer::Get());
		renderer->PushFrameDeletion(meshBuffers.vertexBuffer.allocation, meshBuffers.vertexBuffer.buffer);
		renderer->PushFrameDeletion(meshBuffers.indexBuffer.allocation, meshBuffers.indexBuffer.buffer);
	}

	ManualDeleteMeshAsset::ManualDeleteMeshAsset(ManualDeleteMeshAsset && o) noexcept : meshBuffers(std::move(o.meshBuffers)),name(std::move(o.name)), lods(std::move(o.lods))
	{
	}

	ManualDeleteMeshAsset &ManualDeleteMeshAsset::operator=(ManualDeleteMeshAsset && o) noexcept {
		swap(o);
		return *this;
	}
	void ManualDeleteMeshAsset::swap(ManualDeleteMeshAsset &other) noexcept {
		std::swap(name, other.name);
		std::swap(lods, other.lods);
		std::swap(meshBuffers, other.meshBuffers);
	}
} // namespace Imagine::Vulkan
