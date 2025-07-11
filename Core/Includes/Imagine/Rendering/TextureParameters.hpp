//
// Created by ianpo on 08/07/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Math/ImagePixelType.hpp"

namespace Imagine {
	enum TextureUsage : uint16_t {
		TexUsg_Undefined = 0,
		TexUsg_CpuReadable = BIT(0),
		TexUsg_GraphicsShaders = BIT(1),
		TexUsg_ComputeShaders = BIT(2),
		TexUsg_Sampling = BIT(3),
		TexUsg_Storage = BIT(4),
	};

	static inline constexpr std::string TextureUsageToString(const TextureUsage usg) {
		if (usg == 0) return "Undefined";
		std::string str;
		if (usg & TexUsg_CpuReadable) {str += "CpuReadable"; }
		if (usg & TexUsg_GraphicsShaders) {if(!str.empty()) str += " | "; str += "GraphicsShaders"; }
		if (usg & TexUsg_ComputeShaders) {if(!str.empty()) str += " | "; str += "ComputeShaders"; }
		if (usg & TexUsg_Sampling) {if(!str.empty()) str += " | "; str += "Sampling"; }
		if (usg & TexUsg_Storage) {if(!str.empty()) str += " | "; str += "Storage"; }
		return str;
	}

	static inline bool constexpr TryTextureUsageFromString(const std::string& str, TextureUsage& usg) {
		if(str == "Undefined") {usg = TextureUsage::TexUsg_Undefined; return true;}
		usg = TexUsg_Undefined;
		if (str.find("CpuReadable") != std::string::npos) (uint16_t&)usg |= (uint16_t)TexUsg_CpuReadable;
		if (str.find("GraphicsShaders") != std::string::npos) (uint16_t&)usg |= (uint16_t)TexUsg_GraphicsShaders;
		if (str.find("ComputeShaders") != std::string::npos) (uint16_t&)usg |= (uint16_t)TexUsg_ComputeShaders;
		if (str.find("Sampling") != std::string::npos) (uint16_t&)usg |= (uint16_t)TexUsg_Sampling;
		if (str.find("Storage") != std::string::npos) (uint16_t&)usg |= (uint16_t)TexUsg_Storage;
		return usg != TexUsg_Undefined;
	}
}