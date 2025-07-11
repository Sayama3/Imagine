//
// Created by ianpo on 11/07/2025.
//

#pragma once

#include "Imagine/Assets/AssetField.decl.hpp"
#include "Imagine/Assets/AssetType.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/TypeHelper.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#include <imgui_internal.h>
#endif

#ifdef MGN_IMGUI
	#ifdef MGN_DOUBLE
		inline static constexpr ImGuiDataType_ ImGuiDataType_Real = ImGuiDataType_Double;
	#else
		inline static constexpr ImGuiDataType_ ImGuiDataType_Real = ImGuiDataType_Float;
	#endif
#else
	#ifdef MGN_DOUBLE
		inline static constexpr int ImGuiDataType_Real = 9;
	#else
		inline static constexpr int ImGuiDataType_Real = 8;
	#endif
#endif


namespace Imagine::ThirdParty {

	namespace ImGuiLib {
#ifdef MGN_IMGUI
	static inline constexpr bool c_ImGui{false};
#else
	static inline constexpr bool c_ImGui{true};
#endif

		void CreateContext();
		void InitializeWindow();
		void InitializeRenderer();

		void SetDarkThemeColors();
		void SetLightThemeColors();

		void EnableDocking();
		void DisableDocking();

		bool DockingEnabled();

		void NewFrame();
		void NewRenderFrame();
		void NewWindowFrame();

		void Render();
		void PostRender();

		void Shutdown();
		void ShutdownWindow();
		void ShutdownRenderer();

		void BlockEvents();
		void UnblockEvents();
		void SetEventsBlocked(bool eventBlocked);
		bool EventsBlocked();


		struct ImGuiImage {
			virtual ~ImGuiImage() = default;
			virtual uint64_t GetImGuiID() = 0;
			virtual glm::fvec2 GetSize() = 0;
		};


		bool DragReal(const char *label, Real *v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		bool DragReal2(const char *label, Real *v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		bool DragReal3(const char *label, Real *v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		bool DragReal4(const char *label, Real *v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		bool SliderReal(const char *label, Real *v, Real v_min, Real v_max, const char *format = "%.3f", ImGuiSliderFlags flags = 0);
		bool InputUUID(const char* name, UUID* id, ImGuiInputTextFlags flags = 0);

		template<typename T>
		inline bool RenderData(const char *label, T *data) {
#ifdef MGN_IMGUI
			ImGui::Text("Component '%s' cannot be rendered yet.", NiceTypeName<T>());
#endif
			return false;
		}

		template<typename T>
		static bool DrawAssetField(const char* name, AssetField<T> * v)
		{
#ifdef MGN_IMGUI
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems) {
				return false;
			}
			bool changed = false;
			const AssetType assetType = AssetField<T>::GetStaticType();

			ImGuiContext& g = *GImGui;
			ImGui::BeginGroup();
			ImGui::PushID(name);

			// TODO: Draw a little preview

			UUID handle = v->GetHandle().GetID();
			if(ImGuiLib::InputUUID("##UUID", &handle))
			{
				v->SetHandle(handle);
				changed = true;
			}

			if(ImGui::BeginDragDropTarget())
			{
				auto payloadStr = AssetTypeToPayloadID(assetType);
				MGN_CORE_CASSERT(payloadStr.size() < 32, "The payloadID is too large.");
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(payloadStr.c_str());
				if (payload != nullptr) {
					MGN_CORE_CASSERT(payload->DataSize == sizeof(AssetHandle), "The data is not an AssetHandle");
					auto payloadHandle = *((AssetHandle*)(payload->Data));
					v->SetHandle(payloadHandle);
					changed = true;
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			if(ImGui::Button("Reset")) {
				v->SetHandle(0);
				changed = true;
			}

			ImGui::SameLine(0, g.Style.ItemInnerSpacing.x);
			ImGui::TextEx(name, ImGui::FindRenderedTextEnd(name));

			ImGui::PopID();
			ImGui::EndGroup();

			return changed;
#else
			return false;
#endif
		}

	} // namespace ImGuiLib

} // namespace Imagine::ThirdParty
