//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUTexture2D.hpp"
#include "Imagine/Rendering/CPU/CPUTexture3D.hpp"
// #include "Imagine/Rendering/CPU/CPUCubemap.hpp"
#include <any>

namespace Imagine::Core
{
	class TextureImporter
	{
	public:
		static bool IsTexture2D(const std::filesystem::path& path);
		static Ref<CPUTexture2D> LoadTexture2D(const std::filesystem::path& path);
		static void ExportEditorTexture2D(const AssetMetadata& metadata, Ref<CPUTexture2D> texture);
		static Ref<Asset> ImportTexture2D(const AssetMetadata& metadata);

		// static bool IsCubemap(const std::filesystem::path& path);
		// static void ExportEditorCubemap(const AssetMetadata& metadata, Ref<Cubemap> cubemap);
		// static Ref<Asset> ImportCubemap(const AssetMetadata& metadata);

	private:
		// static Ref<Cubemap> LoadCubemap(const std::filesystem::path& path);
	};
}
