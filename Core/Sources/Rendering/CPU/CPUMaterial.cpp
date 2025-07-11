//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Project/Project.hpp"
#include "Imagine/Assets/AssetManagerBase.hpp"

namespace Imagine {
	Ref<CPUMaterial> CPUMaterial::s_DefaultOpaqueMaterial{nullptr};
	Ref<CPUMaterial> CPUMaterial::s_DefaultTransparentMaterial{nullptr};
	Ref<CPUMaterial> CPUMaterial::s_DefaultLineMaterial{nullptr};

	Ref<CPUMaterial> CPUMaterial::GetDefaultOpaque() {
		return s_DefaultOpaqueMaterial;
	}

	Ref<CPUMaterial> CPUMaterial::GetDefaultTransparent() {
		return s_DefaultTransparentMaterial;
	}

	Ref<CPUMaterial> CPUMaterial::GetDefaultLine() {
		return s_DefaultLineMaterial;
	}

	void CPUMaterial::DestroyDefaultMaterials() {
		if (!s_DefaultOpaqueMaterial || !s_DefaultTransparentMaterial || !s_DefaultLineMaterial) return;

		if (auto active = Project::GetActive()) {
			if (auto assets = active->GetAssetManager()) {
				assets->RemoveAsset(s_DefaultOpaqueMaterial->Handle);
				assets->RemoveAsset(s_DefaultTransparentMaterial->Handle);
				assets->RemoveAsset(s_DefaultLineMaterial->Handle);
			}
		}

		s_DefaultOpaqueMaterial.reset();
		s_DefaultTransparentMaterial.reset();
		s_DefaultLineMaterial.reset();
	}

	void CPUMaterial::InitDefaultMaterials(const AssetHandle vertexShader, const AssetHandle fragmentShader) {
		if (s_DefaultOpaqueMaterial || s_DefaultTransparentMaterial || s_DefaultLineMaterial) {
			s_DefaultOpaqueMaterial->shaders[0] = vertexShader;
			s_DefaultOpaqueMaterial->shaders[4] = fragmentShader;

			s_DefaultTransparentMaterial->shaders[0] = vertexShader;
			s_DefaultTransparentMaterial->shaders[4] = fragmentShader;

			s_DefaultLineMaterial->shaders[0] = vertexShader;
			s_DefaultLineMaterial->shaders[4] = fragmentShader;

			s_DefaultOpaqueMaterial->gpu.reset();
			s_DefaultTransparentMaterial->gpu.reset();
			s_DefaultLineMaterial->gpu.reset();
			return;
		}

		s_DefaultOpaqueMaterial = CreateRef<CPUMaterial>();
		s_DefaultTransparentMaterial = CreateRef<CPUMaterial>();
		s_DefaultLineMaterial = CreateRef<CPUMaterial>();

		s_DefaultOpaqueMaterial->layout = MaterialLayout::GetDefaultLayout();
		s_DefaultOpaqueMaterial->mode = MaterialFilling::Fill;
		s_DefaultOpaqueMaterial->pass = MaterialPass::MainColor;
		s_DefaultOpaqueMaterial->topology = Topology::Triangle;
		s_DefaultOpaqueMaterial->shaders[0] = vertexShader;
		s_DefaultOpaqueMaterial->shaders[4] = fragmentShader;

		s_DefaultLineMaterial->layout = MaterialLayout::GetDefaultLayout();
		s_DefaultLineMaterial->mode = MaterialFilling::Fill;
		s_DefaultLineMaterial->pass = MaterialPass::MainColor;
		s_DefaultLineMaterial->topology = Topology::Line;
		s_DefaultLineMaterial->shaders[0] = vertexShader;
		s_DefaultLineMaterial->shaders[4] = fragmentShader;

		s_DefaultTransparentMaterial->layout = MaterialLayout::GetDefaultLayout();
		s_DefaultTransparentMaterial->mode = MaterialFilling::Fill;
		s_DefaultTransparentMaterial->pass = MaterialPass::Transparent;
		s_DefaultTransparentMaterial->topology = Topology::Triangle;
		s_DefaultTransparentMaterial->shaders[0] = vertexShader;
		s_DefaultTransparentMaterial->shaders[4] = fragmentShader;

		TryRegisterMaterials();
	}

	void CPUMaterial::TryRegisterMaterials() {
		if (const auto active = Project::GetActive()) {
			if (const auto assets = active->GetAssetManager()) {
				assets->AddAsset(s_DefaultOpaqueMaterial);
				assets->AddAsset(s_DefaultTransparentMaterial);
				assets->AddAsset(s_DefaultLineMaterial);
			}
		}
	}
} // namespace Imagine
