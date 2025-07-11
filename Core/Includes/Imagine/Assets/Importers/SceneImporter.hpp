//
// Created by ianpo on 12/04/2024.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Scene/Scene.hpp"
#include <any>

#define SceneExtension ".mgn"

namespace Imagine
{

	class SceneImporter
	{
	public:
		static bool IsScene(const std::filesystem::path& path);
		static Ref<Asset> ImportScene(const AssetMetadata& metadata);
		static void ExportEditorScene(const AssetMetadata& metadata, Ref<Scene> scene);
	};

} // namespace Imagine

