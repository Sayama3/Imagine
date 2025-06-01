//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "WindowParameters.hpp"
#include "Imagine/Core/Size.hpp"

namespace Imagine::Core {

	class Window {
	public:
		static Window* Create(const std::string &windowName, WindowParameters parameters);
		static Window* Initialize(const std::string &windowName, WindowParameters parameters);
		static void Shutdown();
		static Window* Get();
	private:
		static Window* s_Window;
	protected:
		Window() {}
	public:;
		virtual ~Window() {}
		Window(const Window&) = delete;

		Window& operator=(const Window&) = delete;
	public:
		virtual void Update() = 0;
		virtual void SendImGuiCommands() = 0;
	public:
		virtual uint32_t GetWindowWidth() = 0;
		virtual uint32_t GetWindowHeight() = 0;
		virtual Size2 GetWindowSize() = 0;

		virtual uint32_t GetFramebufferWidth() = 0;
		virtual uint32_t GetFramebufferHeight() = 0;
		virtual Size2 GetFramebufferSize() = 0;

		virtual void* GetNativeWindow() = 0;
		virtual bool WindowHasResized() = 0;
		virtual bool ShouldClose() = 0;

		/**
		 * This function allow to obtain a pointer to the SDL3 or GLFW window. NOT the native window ptr.
		 * @return The pointer of the window
		 */
		virtual void* GetWindowPtr() = 0;
	};

}
