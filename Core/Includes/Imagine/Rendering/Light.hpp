//
// Created by ianpo on 09/07/2025.
//

#pragma once
#include "Imagine/Math/Core.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"
namespace Imagine {

	enum LightType : int32_t {
		LIGHT_POINT = 0,
		LIGHT_DIRECTIONAL = 1,
		LIGHT_SPOT = 2,
	};
    static inline constexpr const char* const LightTypeNames[] = { "Point Light", "Directional Light", "SpotLight"};

	struct Light {
		glm::fvec4 color{1,1,1,1};
		glm::fvec4 direction{10,0,0,45*Math::DegToRad};
		glm::fvec3 position{0};
		LightType type{LIGHT_POINT};
	};

	namespace ThirdParty::ImGuiLib {
		template<>
		inline bool RenderData<Light>(const char* label, Light* data) {
			bool changed = false;
#ifdef MGN_IMGUI
			ImGui::PushID(label);
			ImGui::SeparatorText(label);
			changed |= ImGui::ColorEdit3("Color", &data->color.x);
			changed |= ImGui::DragFloat("Intensity", &data->color.w,0.1, 0, FLT_MAX);
			if (data->type != LIGHT_POINT) {
				if (data->type == LIGHT_SPOT) {
					changed |= ImGui::SliderAngle("CutOut", &data->direction.w, 0, 360);
				}
			}
			if (data->type != LIGHT_DIRECTIONAL) {
				changed |= ImGui::DragFloat("Range", &data->direction.x, 0.1, 0, FLT_MAX);
				changed |= ImGui::DragFloat3("Offset", &data->position.x,0.1);
			}
			if (ImGui::BeginCombo("Type", LightTypeNames[data->type]))
			{
				for (int n = 0; n < IM_ARRAYSIZE(LightTypeNames); n++)
				{
					const bool is_selected = (data->type == n);
					if (ImGui::Selectable(LightTypeNames[n], is_selected)) {
						data->type = (LightType) n;
						changed = true;
					}

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopID();
#endif
			return changed;
		}
	}
} // namespace Imagine
