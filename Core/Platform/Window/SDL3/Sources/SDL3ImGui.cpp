//
// Created by Sayama on 28/05/2025.
//


#include <imgui_impl_sdl3.h>
#include "Imagine/Rendering/MgnImGui.hpp"

#include "SDL3Window.hpp"
#include "Imagine/Application/Window.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Rendering/MgnImGui.hpp"

namespace Imagine::Core {
	void MgnImGui::InitializeWindow() {
		MGN_CORE_ASSERT(Imagine::Core::Window::Get(), "Window is not created.");
#ifdef MGN_RENDERER_VULKAN
		ImGui_ImplSDL3_InitForVulkan(static_cast<struct SDL_Window *>(Imagine::Core::Window::Get()->GetWindowPtr()));
#else
#error "The renderer is not implemented for ImGui"
#endif
	}

	void MgnImGui::NewWindowFrame() {
		ImGui_ImplSDL3_NewFrame();
	}
	void MgnImGui::WindowProcessEvents(void* event) {
		ImGui_ImplSDL3_ProcessEvent(static_cast<SDL_Event*>(event));
	}
	void MgnImGui::ShutdownWindow() {
		ImGui_ImplSDL3_Shutdown();
	}
} // namespace Imagine::Core
