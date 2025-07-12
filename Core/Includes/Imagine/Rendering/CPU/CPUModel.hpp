//
// Created by ianpo on 08/07/2025.
//

#pragma once
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Rendering/CPU/CPUShader.hpp"
#include "Imagine/Rendering/CPU/CPUTexture2D.hpp"
#include "Imagine/Rendering/CPU/CPUVirtualTexture.hpp"

#include "Imagine/Scene/Entity.hpp"

namespace Imagine {
	class Scene;
	class CPUMaterialInstance;

	class CPUModel : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Model);
	public:

		struct Node {
			std::string name;
			std::vector<Weak<CPUMesh>> meshes{};
			std::optional<Light> light{std::nullopt};
			Mat4 worldMatrix{Math::Identity<Mat4>()};
			Quat LocalRotation{Math::Identity<Quat>()};
			Vec3 LocalPosition{0};
			Vec3 LocalScale{1};
			std::optional<uint64_t> parent{std::nullopt};
			std::vector<uint64_t> children{};
		};
	public:
		static Ref<CPUModel> LoadModel(const Path &filePath, Scene* coreScene, EntityID parent = EntityID::NullID);
		static Ref<CPUModel> LoadModel(const std::filesystem::path &filePath, Scene* coreScene, EntityID parent = EntityID::NullID);
		static Ref<CPUModel> LoadModel(const Path &filePath);
		static Ref<CPUModel> LoadModel(const std::filesystem::path &filePath);
	public:
		void LoadInScene(Scene* coreScene, EntityID parent = EntityID::NullID);
	public:
		Path modelPath;
		std::vector<Ref<CPUMesh>> Meshes;
		std::vector<Ref<CPUMaterial>> Materials;
		std::vector<Ref<CPUMaterialInstance>> Instances;
		std::vector<Ref<CPUTexture2D>> Textures;
		std::vector<Ref<CPUShader>> Shaders;

		uint64_t RootNode{0};
		std::vector<Node> Nodes{};
	};

} // namespace Imagine
