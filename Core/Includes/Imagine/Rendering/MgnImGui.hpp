//
// Created by Sayama on 28/05/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"

namespace Imagine::Core {
	class MgnImGui {
	private:
		inline static bool s_DockingEnabled { false };
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
	};

	struct ImGuiImage {
		virtual ~ImGuiImage() = default;
		virtual uint64_t GetImGuiID() = 0;
		virtual glm::fvec2 GetSize() = 0;
	};
} // namespace Imagine::Core
