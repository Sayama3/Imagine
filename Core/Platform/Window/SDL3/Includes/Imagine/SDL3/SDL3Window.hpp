#pragma once

#include "Imagine/Application/Window.hpp"
#include "Imagine/Core/Inputs.hpp"

#include <SDL3/SDL.h>


namespace Imagine::SDL3 {
	class SDL3Window final : public Window {
	public:
		SDL3Window(const std::string &windowName, const WindowParameters parameters);
		~SDL3Window() override;

		void Update() override;
		void SendImGuiCommands() override {}

        virtual void SetEventCallback(const EventCallBackFn& callback) override;

		[[nodiscard]] Vec2 GetPosition() override;

		[[nodiscard]] uint32_t GetWindowWidth() override;
		[[nodiscard]] uint32_t GetWindowHeight() override;
		[[nodiscard]] Size2 GetWindowSize() override;

		[[nodiscard]] virtual Rect<> GetWindowRect() override;

		[[nodiscard]] uint32_t GetFramebufferWidth() override;
		[[nodiscard]] uint32_t GetFramebufferHeight() override;
		[[nodiscard]] Size2 GetFramebufferSize() override;
		[[nodiscard]] void *GetNativeWindow() override;

		[[nodiscard]] bool IsMinimized() override;
		[[nodiscard]] bool ShouldClose() override;

		[[nodiscard]] void *GetWindowPtr() override;
	public:
		const KeyboardState& GetKeyboardState() const;
		const MouseState& GetMouseState() const;
	private:
		static bool OnPixelSizeChanged(void *userdata, SDL_Event *event);
	private:
		EventCallBackFn m_EventCallBack{};
		struct SDL_Window *m_Window{nullptr};

		MouseState m_Mouse;
		KeyboardState m_Keyboard;

		bool m_ShouldClose = false;
		bool m_Minimized = false;
	};
} // namespace Imagine
