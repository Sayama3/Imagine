//
// Created by Sayama on 28/05/2025.
//


#include <imgui_impl_sdl3.h>
#include "Imagine/ThirdParty/ImGui.hpp"

#include "Imagine/SDL3/SDL3Window.hpp"
#include "Imagine/Application/Window.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

namespace Imagine::ThirdParty::ImGuiLib {
	void InitializeWindow() {
		MGN_CORE_CASSERT(Imagine::Window::Get(), "Window is not created.");
#ifdef MGN_RENDERER_VULKAN
		ImGui_ImplSDL3_InitForVulkan(static_cast<struct SDL_Window *>(Imagine::Window::Get()->GetWindowPtr()));
#else
#error "The renderer is not implemented for ImGui"
#endif
	}

	void NewWindowFrame() {
		ImGui_ImplSDL3_NewFrame();
	}

	void ShutdownWindow() {
		ImGui_ImplSDL3_Shutdown();
	}
} // namespace Imagine
