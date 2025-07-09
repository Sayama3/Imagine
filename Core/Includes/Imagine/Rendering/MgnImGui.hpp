//
// Created by Sayama on 28/05/2025.
//

#pragma once

#include "Imagine/Core/TypeHelper.hpp"
#include "Imagine/Math/Core.hpp"

#ifdef MGN_IMGUI
#include <imgui.h>
#endif

namespace Imagine::Core {

#ifdef MGN_IMGUI
	static inline constexpr bool c_ImGui{false};
#else
	static inline constexpr bool c_ImGui{true};
#endif

	class MgnImGui {
	private:
		inline static bool s_DockingEnabled { false };
		inline static bool s_EventBlocked { true };
		inline static bool s_IsDarkTheme { true };
	public:
		static void CreateContext();
		static void InitializeWindow();
		static void InitializeRenderer();

		static void SetDarkThemeColors();
		static void SetLightThemeColors();

		static void EnableDocking();
		static void DisableDocking();

		static bool DockingEnabled();

		static void NewFrame();
		static void NewRenderFrame();
		static void NewWindowFrame();

		static void Render();
		static void PostRender();

		static void Shutdown();
		static void ShutdownWindow();
		static void ShutdownRenderer();

		static void BlockEvents();
		static void UnblockEvents();
		static void SetEventsBlocked(bool eventBlocked);
		static bool EventsBlocked();
	};

	struct ImGuiImage {
		virtual ~ImGuiImage() = default;
		virtual uint64_t GetImGuiID() = 0;
		virtual glm::fvec2 GetSize() = 0;
	};

	namespace ImGuiLib {
		 bool DragReal(const char* label, Real* v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		 bool DragReal2(const char* label, Real* v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		 bool DragReal3(const char* label, Real* v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);
		 bool DragReal4(const char* label, Real* v, float v_speed = 1.0f, Real v_min = 0, Real v_max = 0, const char *format = "%.3f", int flags = 0);

		template<typename T>
		inline bool RenderData(const char* label, T* data) {
#ifdef MGN_IMGUI
			ImGui::Text("Component '%s' cannot be rendered yet.", NiceTypeName<T>());
#endif
			return false;
		}
	}
} // namespace Imagine::Core
