//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include <spdlog/spdlog.h>

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

	inline static const std::string ShaderStageToString(const ShaderStage ss) {
		std::string str;
		if (CHECK_SHADER_STAGE_BIT(ss, ShaderStage::Vertex)) {
			str += "Vertex";
		}
		if (CHECK_SHADER_STAGE_BIT(ss, ShaderStage::TessellationControl)) {
			if(!str.empty()) str += " | ";
			str += "TessellationControl";
		}
		if (CHECK_SHADER_STAGE_BIT(ss, ShaderStage::TessellationEvaluation)) {
			if(!str.empty()) str += " | ";
			str += "TessellationEvaluation";
		}
		if (CHECK_SHADER_STAGE_BIT(ss, ShaderStage::Geometry)) {
			if(!str.empty()) str += " | ";
			str += "Geometry";
		}
		if (CHECK_SHADER_STAGE_BIT(ss, ShaderStage::Fragment)) {
			if(!str.empty()) str += " | ";
			str += "Fragment";
		}
		if (CHECK_SHADER_STAGE_BIT(ss, ShaderStage::Compute)) {
			if(!str.empty()) str += " | ";
			str += "Compute";
		}
		return str;
	}

} // namespace Imagine::Core

template<>
struct fmt::formatter<Imagine::Core::ShaderStage> : fmt::formatter<std::string>
{
	auto format(Imagine::Core::ShaderStage value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", Imagine::Core::ShaderStageToString(value));
	}
};
