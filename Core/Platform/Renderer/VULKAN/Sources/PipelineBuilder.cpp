//
// Created by ianpo on 02/06/2025.
//

#include "Imagine/Vulkan/PipelineBuilder.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include "Imagine/Vulkan/VulkanUtils.hpp"

using namespace Imagine;

namespace Imagine::Vulkan {
	PipelineBuilder::PipelineBuilder() {
		Clear();
		m_ShaderStages.reserve(c_ShaderStageCount);
	}

	PipelineBuilder::PipelineBuilder(VkPipelineLayout pipelineLayout) : PipelineBuilder() {
		m_PipelineLayout = pipelineLayout;
	}

	PipelineBuilder::~PipelineBuilder() = default;

	PipelineBuilder &PipelineBuilder::SetInputTopology(const VkPrimitiveTopology topology) {
		m_InputAssembly.topology = topology;
		// TODO: Investigate what is "primitiveRestartEnable".
		m_InputAssembly.primitiveRestartEnable = false;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::SetPolygonMode(VkPolygonMode mode, float lineWidth) {
		m_Rasterizer.polygonMode = mode;
		m_Rasterizer.lineWidth = lineWidth;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::SetCullMode(VkCullModeFlags cullMode, VkFrontFace frontFace) {
		m_Rasterizer.cullMode = cullMode;
		m_Rasterizer.frontFace = frontFace;
		return *this;
	}
	PipelineBuilder &PipelineBuilder::SetMultisamplingNone() {
		m_Multisampling.sampleShadingEnable = VK_FALSE;
		// multisampling defaulted to no multisampling (1 sample per pixel)
		m_Multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_Multisampling.minSampleShading = 1.0f;
		m_Multisampling.pSampleMask = nullptr;
		// no alpha to coverage either
		m_Multisampling.alphaToCoverageEnable = VK_FALSE;
		m_Multisampling.alphaToOneEnable = VK_FALSE;

		return *this;
	}

	PipelineBuilder &PipelineBuilder::DisableBlending() {
		// default write mask
		m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		// no blending
		m_ColorBlendAttachment.blendEnable = VK_FALSE;

		return *this;
	}

	PipelineBuilder &PipelineBuilder::SetColorAttachmentFormat(const VkFormat format) {
		m_ColorAttachmentformat = format;

		m_RenderInfo.colorAttachmentCount = 1;
		m_RenderInfo.pColorAttachmentFormats = &m_ColorAttachmentformat;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::SetDepthFormat(const VkFormat format) {
		m_RenderInfo.depthAttachmentFormat = format;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::DisableDepthTest() {
		m_DepthStencil.depthTestEnable = VK_FALSE;
		m_DepthStencil.depthWriteEnable = VK_FALSE;
		m_DepthStencil.depthCompareOp = VK_COMPARE_OP_NEVER;
		m_DepthStencil.depthBoundsTestEnable = VK_FALSE;
		m_DepthStencil.stencilTestEnable = VK_FALSE;
		m_DepthStencil.front = {};
		m_DepthStencil.back = {};
		m_DepthStencil.minDepthBounds = 0.f;
		m_DepthStencil.maxDepthBounds = 1.f;
		return *this;
	}
	PipelineBuilder &PipelineBuilder::EnableDepthTest(bool depthWriteEnable, VkCompareOp op) {
		m_DepthStencil.depthTestEnable = VK_TRUE;
		m_DepthStencil.depthWriteEnable = depthWriteEnable;
		m_DepthStencil.depthCompareOp = op;
		m_DepthStencil.depthBoundsTestEnable = VK_FALSE;
		m_DepthStencil.stencilTestEnable = VK_FALSE;
		m_DepthStencil.front = {};
		m_DepthStencil.back = {};
		m_DepthStencil.minDepthBounds = 0.f;
		m_DepthStencil.maxDepthBounds = 1.f;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::EnableBlendingAdditive()
	{
		m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_ColorBlendAttachment.blendEnable = VK_TRUE;
		m_ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		m_ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::EnableBlendingAlpha() {
		m_ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		m_ColorBlendAttachment.blendEnable = VK_TRUE;
		m_ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		m_ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		m_ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		m_ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		m_ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		m_ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::SetPipelineLayout(VkPipelineLayout pipelineLayout) {
		m_PipelineLayout = pipelineLayout;
		return *this;
	}

	PipelineBuilder &PipelineBuilder::ClearShaders() {
		m_ShaderStages.clear();
		return *this;
	}

	PipelineBuilder &PipelineBuilder::AddShader(ShaderStage stage, VkShaderModule shader, const char *name /* = "main"*/) {
		m_ShaderStages.push_back(Initializer::PipelineShaderStageCreateInfo(Utils::GetShaderStageFlagsBits(stage), shader, name));
		return *this;
	}

	VkPipeline PipelineBuilder::BuildPipeline(VkDevice device) {
		MGN_CORE_CASSERT(m_PipelineLayout, "Cannot build a pipeline without a pipeline layout.");
		if (!m_PipelineLayout) return VK_NULL_HANDLE;

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;

		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// setup dummy color blending. We arent using transparent objects yet
		// the blending is just "no blend", but we do write to the color attachment
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;

		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &m_ColorBlendAttachment;

		// completely clear VertexInputStateCreateInfo, as we have no need for it yet
		VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		// build the actual pipeline
		// we now use all of the info structs we have been writing into,
		// into this one to create the pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		// connect the renderInfo to the pNext extension mechanism
		pipelineInfo.pNext = &m_RenderInfo;

		pipelineInfo.stageCount = static_cast<uint32_t>(m_ShaderStages.size());
		pipelineInfo.pStages = m_ShaderStages.data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &m_InputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &m_Rasterizer;
		pipelineInfo.pMultisampleState = &m_Multisampling;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDepthStencilState = &m_DepthStencil;
		pipelineInfo.layout = m_PipelineLayout;

		// Creating the dynamic viewport & scissor.
		const VkDynamicState state[] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR, /*VK_DYNAMIC_STATE_LINE_WIDTH*/ };

		VkPipelineDynamicStateCreateInfo dynamicInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO};
		dynamicInfo.pDynamicStates = state;
		dynamicInfo.dynamicStateCount = 2;

		pipelineInfo.pDynamicState = &dynamicInfo;

		// It's easy to error out on create graphics pipeline,
		// so we handle it a bit better than the common VK_CHECK case
		VkPipeline newPipeline;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &newPipeline) != VK_SUCCESS) {
			MGN_CORE_ERROR("[Vulkan] Failed to create pipeline.");
			return VK_NULL_HANDLE; // failed to create graphics pipeline
		}
		return newPipeline;
	}

	void PipelineBuilder::Clear() {
		// clear all of the structs we need back to 0 with their correct stype

		m_InputAssembly = {.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};

		m_Rasterizer = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO};

		m_ColorBlendAttachment = {};

		m_Multisampling = {.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO};

		m_PipelineLayout = {};

		m_DepthStencil = {.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO};

		m_RenderInfo = {.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO};

		m_ColorAttachmentformat = VK_FORMAT_UNDEFINED;

		m_ShaderStages.clear();
	}

} // namespace Imagine::Vulkan
