//
// Created by Sayama on 29/04/2025.
//

#pragma once
#include "WindowParameters.hpp"

namespace Imagine::Core {

	class Window {
	public:
		static Window* CreateWindow(const std::string &windowName, WindowParameters parameters);
	protected:
		Window() {}
	public:;
		virtual ~Window() {}
		Window(const Window&) = delete;

		Window& operator=(const Window&) = delete;
	public:
		virtual void Update() = 0;
	public:
		virtual uint32_t GetWindowWidth() = 0;
		virtual uint32_t GetWindowHeight() = 0;

		virtual uint32_t GetFramebufferWidth() = 0;
		virtual uint32_t GetFramebufferHeight() = 0;

		virtual void* GetNativeWindow() = 0;
		virtual bool WindowHasResized() = 0;
	};

}
