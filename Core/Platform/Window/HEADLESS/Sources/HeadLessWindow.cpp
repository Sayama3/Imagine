//
// Created by Sayama on 29/04/2025.
//

#include "HeadLessWindow.hpp"

namespace Imagine {
	HeadLessWindow::HeadLessWindow(const std::string &windowName, const WindowParameters parameters) {}

	HeadLessWindow::~HeadLessWindow() {}

	void HeadLessWindow::Update() {
		// TODO: Poll Events ?
	}

	uint32_t HeadLessWindow::GetWindowWidth() {
		return 0;
	}

	uint32_t HeadLessWindow::GetWindowHeight() {
		return 0;
	}

	uint32_t HeadLessWindow::GetFramebufferWidth() {
		return 0;
	}

	uint32_t HeadLessWindow::GetFramebufferHeight() {
		return 0;
	}

	void *HeadLessWindow::GetNativeWindow() {
		return nullptr;
	}

	bool HeadLessWindow::WindowHasResized() {
		return false;
	}

	bool HeadLessWindow::ShouldClose() {
		return false;
	}
} // namespace Imagine
