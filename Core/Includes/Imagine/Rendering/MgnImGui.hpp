//
// Created by Sayama on 28/05/2025.
//

#pragma once

namespace Imagine::Core {
	class MgnImGui {
	public:
		static void CreateContext();
		static void InitializeWindow();
		static void InitializeRenderer();

		static void NewFrame();
		static void NewRenderFrame();
		static void NewWindowFrame();

		static void Render();

		static void Shutdown();
		static void ShutdownWindow();
		static void ShutdownRenderer();
	};
} // namespace Imagine::Core
