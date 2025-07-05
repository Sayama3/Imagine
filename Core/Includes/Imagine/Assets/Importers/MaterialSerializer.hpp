//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include <any>

#define MaterialExtension ".mgn"

namespace Imagine::Core
{

	class MaterialSerializer
	{
	public:
		static bool IsMaterial(const std::filesystem::path& path);
		static void ExportReadableMaterial(const AssetMetadata& metadata, Ref<CPUMaterial> material);
		static Ref<Asset> ImportMaterial(const AssetMetadata& metadata);
	};

} // namespace Imagine::Core

