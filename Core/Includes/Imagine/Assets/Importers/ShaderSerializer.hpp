//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUShader.hpp"
#include <any>


#define ShaderExtension ".shd.mgn"

namespace Imagine::Core
{

	class ShaderSerializer
	{
	public:
		static bool IsShaderSource(const std::filesystem::path& path);
		static Ref<Asset> ImportReadableShaderSource(const AssetMetadata& metadata);
		static void ExportReadableShaderSource(const AssetMetadata& metadata, Ref<CPUShader> source);
	};

} // namespace Imagine::Core

