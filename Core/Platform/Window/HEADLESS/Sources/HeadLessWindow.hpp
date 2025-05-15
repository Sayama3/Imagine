//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "Imagine/Application/Window.hpp"

namespace Imagine::Core {
	class HeadLessWindow final : public Window {
	public:
		HeadLessWindow(const std::string& windowName, const WindowParameters parameters);
		virtual ~HeadLessWindow() override;

		virtual void Update() override;

		virtual uint32_t GetWindowWidth() override;
		virtual uint32_t GetWindowHeight() override;

		virtual uint32_t GetFramebufferWidth() override;
		virtual uint32_t GetFramebufferHeight() override;
		virtual void * GetNativeWindow() override;

		virtual bool WindowHasResized() override;
		virtual bool ShouldClose() override;
	private:

	};
}
