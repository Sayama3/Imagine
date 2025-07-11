//
// Created by ianpo on 01/06/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include <spdlog/spdlog.h>

#define CHECK_SHADER_STAGE_BIT(stage1, stage2) ((uint16_t) stage1 & (uint16_t) stage2)

namespace Imagine {

	/**
	 * Enum flag to designate one or multiple shader stage.
	 */
	enum class ShaderStage : uint16_t {
		None = 0,
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

	inline static constexpr std::string ShaderStageToString(const ShaderStage ss) {
		std::string str;
		if (ss == ShaderStage::None) return "None";

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

	inline static constexpr bool TryShaderStageFromString(const std::string& str, ShaderStage& val) {
		val = ShaderStage::None;
		if (str == "None") return true;

		if (str.find("Vertex") != std::string::npos) val = val | ShaderStage::Vertex;
		if (str.find("TessellationControl") != std::string::npos) val = val | ShaderStage::TessellationControl;
		if (str.find("TessellationEvaluation") != std::string::npos) val = val | ShaderStage::TessellationEvaluation;
		if (str.find("Geometry") != std::string::npos) val = val | ShaderStage::Geometry;
		if (str.find("Fragment") != std::string::npos) val = val | ShaderStage::Fragment;
		if (str.find("Compute") != std::string::npos) val = val | ShaderStage::Compute;

		return val != ShaderStage::None;
	}

	inline static constexpr ShaderStage ShaderStageFromString(const std::string& str) {
		ShaderStage val{ShaderStage::None};
		TryShaderStageFromString(str, val);
		return val;
	}

} // namespace Imagine

template<>
struct fmt::formatter<Imagine::ShaderStage> : fmt::formatter<std::string>
{
	auto format(Imagine::ShaderStage value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", Imagine::ShaderStageToString(value));
	}
};
