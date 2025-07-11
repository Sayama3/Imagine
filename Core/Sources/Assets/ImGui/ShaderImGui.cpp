//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/ImGui/ShaderImGui.hpp"
#include "Imagine/Assets/Importers/ShaderSerializer.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine {

	bool ShaderImGui::ShaderTypeCombo(const char *label, ShaderStage *shaderType) {
		bool changed = false;
#ifdef MGN_IMGUI
		std::string shaderTypeName = ShaderStageToString(*shaderType);

		if (ImGui::BeginCombo("Shader Type", shaderTypeName.c_str())) {
			for (int i = 0; i < 6; ++i) {
				ShaderStage type = (ShaderStage) BIT(i);
				const bool is_selected = type == (*shaderType);
				if (ImGui::Selectable(ShaderStageToString(type).c_str(), is_selected)) {
					changed |= (*shaderType) != type;
					(*shaderType) = type;
					// TODO: Export
				}

				if (is_selected) ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
#endif
		return changed;
	}
	bool ShaderImGui::OnShaderSourceImGui(Ref<Asset> asset) {
		return false;
	}

	bool ShaderImGui::OnFileShaderSourceImGui(const Ref<CPUFileShader> &shaderSource) {
		bool changed = false;

		return changed;
	}

	bool ShaderImGui::OnMemoryShaderSourceImGui(const Ref<CPUMemoryShader> &shaderSource) {
		bool changed = false;

		return changed;
	}

	bool ShaderImGui::OnGraphicShaderImGui(Ref<Asset> asset) {
		return false;
	}

	bool ShaderImGui::OnMenuShaderImGui(AssetMetadata asset) {
		return false;
	}


	bool ShaderImGui::OnComputeShaderImGui(Ref<Asset> asset) {
		return false;
	}
} // namespace Imagine
