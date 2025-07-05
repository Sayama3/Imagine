//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUShader.hpp"
#include <any>


#define ShaderExtension ".mgn"
#define ComputeShaderExtension ".mgn"

namespace Imagine::Core
{

	class ShaderSerializer
	{
	public:
		static bool IsShaderSource(const std::filesystem::path& path);
		static Ref<Asset> ImportShaderSource(const AssetMetadata& metadata);
		static void ExportEditorShaderSource(const AssetMetadata& metadata, Ref<CPUShader> source);
	};

} // namespace Imagine::Core

