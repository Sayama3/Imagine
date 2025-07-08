//
// Created by ianpo on 08/07/2025.
//

#pragma once
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Rendering/CPU/CPUTexture2D.hpp"
#include "Imagine/Rendering/CPU/CPUVirtualTexture.hpp"

#include "Imagine/Scene/Entity.hpp"

namespace Imagine::Core {
	class Scene;
	class CPUMaterialInstance;

	class CPUModel : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Model);
	public:
		static Ref<CPUModel> LoadModel(const Path &filePath, Core::Scene* coreScene, Core::EntityID parent = EntityID::NullID);
	public:
		Path modelPath;
		std::vector<Ref<CPUMesh>> Meshes;
		std::vector<Ref<CPUMaterial>> Materials;
		std::vector<Ref<CPUMaterialInstance>> Instances;
		std::vector<Ref<CPUTexture2D>> Textures;
	};

} // namespace Imagine::Core
