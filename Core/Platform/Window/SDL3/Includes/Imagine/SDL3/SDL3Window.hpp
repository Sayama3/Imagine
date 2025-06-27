#pragma once

#include "Imagine/Application/Window.hpp"
#include "Imagine/SDL3/SDL3Inputs.hpp"


namespace Imagine::SDL3 {
	class SDL3Window final : public Core::Window {
	public:
		SDL3Window(const std::string &windowName, const Core::WindowParameters parameters);
		~SDL3Window() override;

		void Update() override;
		void SendImGuiCommands() override {}

        virtual void SetEventCallback(const EventCallBackFn& callback) override;

		[[nodiscard]] Vec2 GetPosition() override;

		[[nodiscard]] uint32_t GetWindowWidth() override;
		[[nodiscard]] uint32_t GetWindowHeight() override;
		[[nodiscard]] Core::Size2 GetWindowSize() override;

		[[nodiscard]] virtual Rect<> GetWindowRect() override;

		[[nodiscard]] uint32_t GetFramebufferWidth() override;
		[[nodiscard]] uint32_t GetFramebufferHeight() override;
		[[nodiscard]] Core::Size2 GetFramebufferSize() override;
		[[nodiscard]] void *GetNativeWindow() override;

		[[nodiscard]] bool IsMinimized() override;
		[[nodiscard]] bool ShouldClose() override;

		[[nodiscard]] void *GetWindowPtr() override;
	private:
		static void framebufferResizeCallback(void *window, int width, int height);

	private:
		EventCallBackFn m_EventCallBack{};
		struct SDL_Window *m_Window{nullptr};
		bool m_ShouldClose = false;
		bool m_Minimized = false;
	};
} // namespace Imagine::Core
