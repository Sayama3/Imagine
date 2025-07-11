//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include "Imagine/Assets/ImGui/TextureImGui.hpp"

namespace Imagine {

	bool TextureImGui::OnTexture2DImGui(Ref<Asset> asset) {
		return false;
	}

	bool TextureImGui::OnTexture3DImGui(Ref<Asset> asset) {
		return false;
	}

	bool TextureImGui::OnCubemapImGui(Ref<Asset> asset) {
		return false;
	}

}
