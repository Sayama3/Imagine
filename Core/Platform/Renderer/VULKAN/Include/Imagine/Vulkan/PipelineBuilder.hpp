//
// Created by ianpo on 02/06/2025.
//

#pragma once

#include <vulkan/vulkan_core.h>


#include "Imagine/Rendering/ShaderParameters.hpp"
#include "Imagine/Vulkan/Vulkan.hpp"

namespace Imagine::Vulkan {

	class PipelineBuilder {
	public:
		PipelineBuilder();
		PipelineBuilder(VkPipelineLayout pipelineLayout);
		~PipelineBuilder();

	public:
		PipelineBuilder &ClearShaders();
		PipelineBuilder &SetInputTopology(VkPrimitiveTopology topology);
		PipelineBuilder &SetPolygonMode(VkPolygonMode mode, float lineWidth = 1.0f);
		PipelineBuilder &SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace);

		PipelineBuilder &SetMultisamplingNone();
		PipelineBuilder &DisableBlending();

		PipelineBuilder &SetColorAttachmentFormat(VkFormat format);
		PipelineBuilder &SetDepthFormat(VkFormat format);

		PipelineBuilder &DisableDepthTest();
		PipelineBuilder &EnableDepthTest(bool depthWriteEnable, VkCompareOp op);
		PipelineBuilder &EnableBlendingAdditive();
		PipelineBuilder &EnableBlendingAlpha();

		PipelineBuilder &SetPipelineLayout(VkPipelineLayout pipelineLayout);

		/**
		 * A function to add a shader
		 * @param stage  The shader stages the shader should be added to.
		 * @param shader The vulkan shader module to set.
		 */
		PipelineBuilder &AddShader(ShaderStage stage, VkShaderModule shader, const char *name = "main");

		VkPipeline BuildPipeline(VkDevice device);
		void Clear();

	public:
		std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

		VkPipelineInputAssemblyStateCreateInfo m_InputAssembly;
		VkPipelineRasterizationStateCreateInfo m_Rasterizer;
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachment;
		VkPipelineMultisampleStateCreateInfo m_Multisampling;
		VkPipelineLayout m_PipelineLayout;
		VkPipelineDepthStencilStateCreateInfo m_DepthStencil;
		VkPipelineRenderingCreateInfo m_RenderInfo;
		VkFormat m_ColorAttachmentformat;
	};

} // namespace Imagine::Vulkan
