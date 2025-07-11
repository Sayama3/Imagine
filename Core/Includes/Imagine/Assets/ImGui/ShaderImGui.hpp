//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUShader.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine
{

	class ShaderImGui
	{
	public:
		static bool ShaderTypeCombo(const char* label, ShaderStage* shaderType);

		static bool OnShaderSourceImGui(Ref<Asset> asset);

		static bool OnGraphicShaderImGui(Ref<Asset> asset);
		static bool OnComputeShaderImGui(Ref<Asset> asset);

		static bool OnMenuShaderImGui(AssetMetadata asset);
	private:
		static bool OnFileShaderSourceImGui(const Ref<CPUFileShader>& shaderSource);
		static bool OnMemoryShaderSourceImGui(const Ref<CPUMemoryShader>& shaderSource);
	};

} // namespace Imagine

