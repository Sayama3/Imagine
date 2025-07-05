//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Renderer/Shader.hpp"
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
		static void ExportEditorShaderSource(const AssetMetadata& metadata, Ref<ShaderSource> source);

		static bool IsGraphicShader(const std::filesystem::path& path);
		static void ExportEditorGraphicShader(const AssetMetadata& metadata, Ref<Shader> shader);
		static Ref<Asset> ImportGraphicShader(const AssetMetadata& metadata);

		static bool IsComputeShader(const std::filesystem::path& path);
		static void ExportEditorComputeShader(const AssetMetadata& metadata, Ref<ComputeShader> shader);
		static Ref<Asset> ImportComputeShader(const AssetMetadata& metadata);
	};

} // namespace Imagine::Core

