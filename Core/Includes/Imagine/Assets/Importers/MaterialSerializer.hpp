//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include <any>
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterial.hpp"

#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"

#define MaterialExtension ".mgn"

namespace Imagine::Core
{

	class MaterialSerializer
	{
	public:
		static bool IsMaterial(const std::filesystem::path& path);
		static void ExportReadableMaterial(const AssetMetadata& metadata, Ref<CPUMaterial> material);
		static Ref<Asset> ImportReadableMaterial(const AssetMetadata& metadata);

		static bool IsMaterialInstance(const std::filesystem::path& path);
		static void ExportReadableMaterialInstance(const AssetMetadata& metadata, Ref<CPUMaterialInstance> instance);
		static Ref<Asset> ImportReadableMaterialInstance(const AssetMetadata& metadata);
	};

} // namespace Imagine::Core

