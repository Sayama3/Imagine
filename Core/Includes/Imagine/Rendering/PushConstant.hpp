//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"

namespace Imagine::Core {

	struct PushConstant {
		template<typename T>
		static PushConstant CreateFrom(ShaderStage stage = ShaderStage::All, uint32_t offset = 0);
		uint32_t offset;
		uint32_t size;
		ShaderStage stage;
	};

	template<typename T>
	PushConstant PushConstant::CreateFrom(const ShaderStage stage, const uint32_t offset) {
		return PushConstant {
			offset,
			sizeof(T),
			stage,
		};
	}

} // namespace Imagine::Core
