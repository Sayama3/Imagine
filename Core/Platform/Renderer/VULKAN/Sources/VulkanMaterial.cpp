//
// Created by ianpo on 13/06/2025.
//

#include "Imagine/Vulkan/VulkanMaterial.hpp"

#include "Imagine/Vulkan/PipelineBuilder.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Vulkan/VulkanUtils.hpp"

using namespace Imagine;

using namespace Imagine;

namespace Imagine::Vulkan {
	uint64_t VulkanMaterialInstance::s_id{0};

	VulkanMaterialInstance::VulkanMaterialInstance() : passType(MaterialPass::MainColor) {
		id = ++s_id;
	}

	VulkanMaterialInstance::~VulkanMaterialInstance() {
		if (deleter) {
			static_cast<VulkanRenderer*>(Renderer::Get())->PushFrameDeletion(deleter);
			deleter.reset();
		}
	}

	uint64_t VulkanMaterialInstance::GetID() {
		return id;
	}

	VulkanMaterial::VulkanMaterial() = default;

	VulkanMaterial::~VulkanMaterial() {
		if (autoDelete) {
			VulkanRenderer* renderer = dynamic_cast<VulkanRenderer*>(Renderer::Get());
			renderer->PushDeletion(pipeline.layout);
			renderer->PushDeletion(pipeline.pipeline);
			for (auto& set : materialLayouts) {
				renderer->PushDeletion(set);
				// vkDestroyDescriptorSetLayout(renderer->GetDevice(), set, nullptr);
			}
			materialLayouts.clear();
		}
	}

	uint64_t VulkanMaterial::GetID() {
		return reinterpret_cast<uint64_t>(pipeline.pipeline);
	}
	void GLTFMetallicRoughness::BuildPipeline(VulkanRenderer* renderer) {
		MGN_CORE_CASSERT(renderer, "The current renderer is not a Vulkan renderer.");

		opaque = CreateRef<VulkanMaterial>();
		transparent = CreateRef<VulkanMaterial>();

		opaque->autoDelete = false;
		transparent->autoDelete = false;

		// TODO: move the shader paths to parameters or change the way we obtains the shader.
		//  Taking into account the fact that later the shader will come from data blob.

		VkShaderModule meshFragShader;
		if (!Utils::LoadShaderModule("Assets/mesh.frag.spv", renderer->GetDevice(), &meshFragShader)) {
			MGN_CORE_ERROR("Error when building the mesh fragment shader module");
		}

		VkShaderModule meshVertexShader;
		if (!Utils::LoadShaderModule("Assets/mesh.vert.spv", renderer->GetDevice(), &meshVertexShader)) {
			MGN_CORE_ERROR("Error when building the mesh vertex shader module");
		}


		VkPushConstantRange matrixRange{};
		matrixRange.offset = 0;
		matrixRange.size = sizeof(GPUDrawPushConstants);
		matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;


		DescriptorLayoutBuilder layoutBuilder; // The main textures for a PBR rendering.
		layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); // Material Uniform Buffer
		layoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // Color image
		layoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // Metal image
		//layoutBuilder.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // Roughness image
		//layoutBuilder.AddBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // Normal image
		//layoutBuilder.AddBinding(5, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // AO image

		auto materialLayout = layoutBuilder.Build(renderer->GetDevice(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

		opaque->materialLayouts.push_back(materialLayout);
		transparent->materialLayouts.push_back(materialLayout);

		VkDescriptorSetLayout layouts[] = {renderer->GetGPUSceneDescriptorLayout(), materialLayout};

		VkPipelineLayoutCreateInfo meshLayoutInfo = Initializer::PipelineLayoutCreateInfo();
		meshLayoutInfo.setLayoutCount = 2;
		meshLayoutInfo.pSetLayouts = layouts;

		meshLayoutInfo.pushConstantRangeCount = 1;
		meshLayoutInfo.pPushConstantRanges = &matrixRange;

		VkPipelineLayout newLayout;
		VK_CHECK(vkCreatePipelineLayout(renderer->GetDevice(), &meshLayoutInfo, nullptr, &newLayout));

		// TODO: Change the layout with dedicated layout for each.
		opaque->pipeline.layout = newLayout;
		transparent->pipeline.layout = newLayout;

		PipelineBuilder builder = PipelineBuilder()
										  .SetPipelineLayout(newLayout)
										  // Parameters
										  .AddShader(ShaderStage::Vertex, meshVertexShader)
										  .AddShader(ShaderStage::Fragment, meshFragShader)
										  // .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
										  .SetPolygonMode(VK_POLYGON_MODE_FILL)
										  .SetCullMode(VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_CLOCKWISE)
										  .SetMultisamplingNone()
										  .DisableBlending()
										  .EnableDepthTest(true, VK_COMPARE_OP_LESS_OR_EQUAL)
										  // Render Format
										  .SetColorAttachmentFormat(renderer->GetColorImageFormat())
										  .SetDepthFormat(renderer->GetDepthImageFormat());
		switch (topology) {
			case Topology::Triangle:
				builder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
				break;
			case Topology::Line:
				builder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
				break;
			case Topology::Point:
				MGN_CORE_CASSERT(false, "The point topology is not supported yet.");
				builder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
				break;
		}
		opaque->pipeline.pipeline = builder.BuildPipeline(renderer->GetDevice());

		builder.EnableBlendingAdditive();
		builder.EnableDepthTest(false, VK_COMPARE_OP_LESS_OR_EQUAL);

		transparent->pipeline.pipeline = builder.BuildPipeline(renderer->GetDevice());

		vkDestroyShaderModule(renderer->GetDevice(), meshFragShader, nullptr);
		vkDestroyShaderModule(renderer->GetDevice(), meshVertexShader, nullptr);
	}

	void GLTFMetallicRoughness::ClearResources(VkDevice device) {
		vkDestroyPipeline(device,opaque->pipeline.pipeline,nullptr);
		if (opaque->pipeline.pipeline != transparent->pipeline.pipeline) vkDestroyPipeline(device,transparent->pipeline.pipeline,nullptr);

		vkDestroyPipelineLayout(device,opaque->pipeline.layout,nullptr);
		if (opaque->pipeline.layout != transparent->pipeline.layout) vkDestroyPipelineLayout(device,transparent->pipeline.layout,nullptr);

		for (auto& layout : opaque->materialLayouts) {
			vkDestroyDescriptorSetLayout(device, layout, nullptr);
		}

		opaque.reset();
		transparent.reset();
	}

	VulkanMaterialInstance GLTFMetallicRoughness::WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources &resources, DescriptorAllocatorGrowable & descriptorAllocator) {
		VulkanMaterialInstance instance;

		instance.passType = pass;

		if (pass == MaterialPass::Transparent) {
			instance.material = transparent;
		}
		else {
			instance.material = opaque;
		}

		if (auto mat = instance.material.lock()) {

			instance.materialSets.push_back(descriptorAllocator.Allocate(device, mat->materialLayouts.front()));

			writer.Clear();
			writer.WriteBuffer(0, resources.dataBuffer, sizeof(MaterialConstants), resources.dataBufferOffset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
			writer.WriteImage(1, resources.colorImage.imageView, resources.colorSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			writer.WriteImage(2, resources.metalImage.imageView, resources.metalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			// writer.WriteImage(3, resources.roughImage.imageView, resources.roughSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			// writer.WriteImage(4, resources.normalImage.imageView, resources.normalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			// writer.WriteImage(5, resources.aoImage.imageView, resources.aoSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
			//TODO: Write a lot of other things.
			writer.UpdateSet(device, instance.materialSets.front());
		}
		return instance;
	}

} // namespace Imagine::Vulkan
