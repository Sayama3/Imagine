//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"

#define CHECK_SHADER_STAGE_BIT(stage1, stage2) ((uint16_t) stage1 & (uint16_t) stage2)

namespace Imagine::Core {

	/**
	 * Enum flag to designate one or multiple shader stage.
	 */
	enum class ShaderStage : uint16_t {
		Vertex = BIT(0),
		TessellationControl = BIT(1),
		TessellationEvaluation = BIT(2),
		Geometry = BIT(3),
		Fragment = BIT(4),
		Compute = BIT(5),
		AllGraphics = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment,
		All = Vertex | TessellationControl | TessellationEvaluation | Geometry | Fragment | Compute,
	};

	static inline constexpr uint64_t c_ShaderStageCount{6};

	inline ShaderStage operator|(ShaderStage a, ShaderStage b) {
		return static_cast<ShaderStage>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
	}

} // namespace Imagine::Core
