//
// Created by ianpo on 13/06/2025.
//

#include "Imagine/Vulkan/VulkanMaterial.hpp"

#include "Imagine/Vulkan/PipelineBuilder.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Vulkan/VulkanUtils.hpp"

using namespace Imagine::Core;

namespace Imagine::Vulkan {
	void GLTFMetallicRoughness::BuildPipeline(VulkanRenderer* renderer) {
		MGN_CORE_ASSERT(renderer, "The current renderer is not a Vulkan renderer.");

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


		DescriptorLayoutBuilder layoutBuilder;
		layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER); // Material Uniform Buffer
		layoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // Color image
		layoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER); // MetalRoughness image

		materialLayout = layoutBuilder.Build(renderer->GetDevice(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);

		VkDescriptorSetLayout layouts[] = {renderer->GetGPUSceneDescriptorLayout(), materialLayout};

		VkPipelineLayoutCreateInfo meshLayoutInfo = Initializer::PipelineLayoutCreateInfo();
		meshLayoutInfo.setLayoutCount = 2;
		meshLayoutInfo.pSetLayouts = layouts;

		meshLayoutInfo.pushConstantRangeCount = 1;
		meshLayoutInfo.pPushConstantRanges = &matrixRange;

		VkPipelineLayout newLayout;
		VK_CHECK(vkCreatePipelineLayout(renderer->GetDevice(), &meshLayoutInfo, nullptr, &newLayout));

		// TODO: Change the layout with dedicated layout for each.
		opaquePipeline.layout = newLayout;
		transparentPipeline.layout = newLayout;

		PipelineBuilder builder = PipelineBuilder()
										  .SetPipelineLayout(newLayout)
										  // Parameters
										  .AddShader(Core::ShaderStage::Vertex, meshVertexShader)
										  .AddShader(Core::ShaderStage::Fragment, meshFragShader)
										  // .SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
										  .SetPolygonMode(VK_POLYGON_MODE_FILL)
										  .SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE)
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
				builder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_POINT_LIST);
				break;
		}
		opaquePipeline.pipeline = builder.BuildPipeline(renderer->GetDevice());

		builder.EnableBlendingAdditive();
		builder.EnableDepthTest(false, VK_COMPARE_OP_LESS_OR_EQUAL);

		transparentPipeline.pipeline = builder.BuildPipeline(renderer->GetDevice());

		vkDestroyShaderModule(renderer->GetDevice(), meshFragShader, nullptr);
		vkDestroyShaderModule(renderer->GetDevice(), meshVertexShader, nullptr);
	}

	void GLTFMetallicRoughness::ClearResources(VkDevice device) {
		vkDestroyPipeline(device,opaquePipeline.pipeline,nullptr);
		if (opaquePipeline.pipeline != transparentPipeline.pipeline) vkDestroyPipeline(device,transparentPipeline.pipeline,nullptr);

		vkDestroyPipelineLayout(device,opaquePipeline.layout,nullptr);
		if (opaquePipeline.layout != transparentPipeline.layout) vkDestroyPipelineLayout(device,transparentPipeline.layout,nullptr);

		vkDestroyDescriptorSetLayout(device, materialLayout, nullptr);
	}

	VulkanMaterialInstance GLTFMetallicRoughness::WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources &resources, DescriptorAllocatorGrowable & descriptorAllocator) {
		VulkanMaterialInstance instance;

		instance.passType = pass;

		if (pass == MaterialPass::Transparent) {
			instance.pipeline = &transparentPipeline;
		}
		else {
			instance.pipeline = &opaquePipeline;
		}

		instance.materialSet = descriptorAllocator.Allocate(device, materialLayout);

		writer.Clear();
		writer.WriteBuffer(0, resources.dataBuffer, sizeof(MaterialConstants), resources.dataBufferOffset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.WriteImage(1, resources.colorImage.imageView, resources.colorSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		writer.WriteImage(2, resources.metalImage.imageView, resources.metalSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		//TODO: Write a lot of other things.
		writer.UpdateSet(device, instance.materialSet);

		return instance;
	}

} // namespace Imagine::Vulkan
