//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/Size.hpp"
#include "Imagine/Events/Event.hpp"
#include "WindowParameters.hpp"

namespace Imagine {


	class Window {
	public:
        using EventCallBackFn = std::function<void(Imagine::Event&, bool fromNormalEventQueue)>;
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
        virtual void SetEventCallback(const EventCallBackFn& callback) = 0;
	public:
		[[nodiscard]] virtual Vec2 GetPosition() = 0;
		[[nodiscard]] virtual uint32_t GetWindowWidth() = 0;
		[[nodiscard]] virtual uint32_t GetWindowHeight() = 0;
		[[nodiscard]] virtual Size2 GetWindowSize() = 0;

		[[nodiscard]] virtual Rect<> GetWindowRect() = 0;

		[[nodiscard]] virtual uint32_t GetFramebufferWidth() = 0;
		[[nodiscard]] virtual uint32_t GetFramebufferHeight() = 0;
		[[nodiscard]] virtual Size2 GetFramebufferSize() = 0;

		[[nodiscard]] virtual bool IsMinimized() = 0;

		[[nodiscard]] virtual void* GetNativeWindow() = 0;
		[[nodiscard]] virtual bool ShouldClose() = 0;

		/**
		 * This function allow to obtain a pointer to the SDL3 or GLFW window. NOT the native window ptr.
		 * @return The pointer of the window
		 */
		[[nodiscard]] virtual void* GetWindowPtr() = 0;
	};

}
