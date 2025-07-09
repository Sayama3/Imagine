//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Rendering/MaterialComponents.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"

#include "Imagine/Rendering/GPU/GPUMaterial.hpp"

namespace Imagine::Core {

	class CPUMaterial final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Material);
	private:
		static Ref<CPUMaterial> s_DefaultOpaqueMaterial;
		static Ref<CPUMaterial> s_DefaultTransparentMaterial;
		static Ref<CPUMaterial> s_DefaultLineMaterial;
	public:
		static Ref<CPUMaterial> GetDefaultOpaque();
		static Ref<CPUMaterial> GetDefaultTransparent();
		static Ref<CPUMaterial> GetDefaultLine();
		static void DestroyDefaultMaterials();
		static void InitDefaultMaterials(AssetHandle vertexShader, AssetHandle fragmentShader);
		static void TryRegisterMaterials();
	public:
		MaterialLayout layout;
		std::array<AssetHandle, 5> shaders;
		MaterialPass pass = MaterialPass::MainColor;
		Topology topology = Topology::Triangle;
		MaterialFilling mode = MaterialFilling::Fill;
		Ref<GPUMaterial> gpu{nullptr};
	};

} // namespace Imagine::Core
