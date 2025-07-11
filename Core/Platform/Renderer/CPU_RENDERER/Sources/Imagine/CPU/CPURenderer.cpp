
//
// Created by Sayama on 23/05/2025.
//

#include "Imagine/CPU/CPURenderer.hpp"


using namespace Imagine;

namespace Imagine::CPU {
	CPURenderer::CPURenderer(const ApplicationParameters &appParams) :
		Renderer() {}
	CPURenderer::~CPURenderer() {}

	void CPURenderer::Draw() {
		MGN_CORE_CASSERT(false, "CPU Renderer not implemented.");
	}
	void CPURenderer::SendImGuiCommands() {
		MGN_CORE_CASSERT(false, "CPU Renderer not implemented.");
	}
	void CPURenderer::PrepareShutdown() {
		MGN_CORE_CASSERT(false, "CPU Renderer not implemented.");
	}
} // namespace Imagine::CPU
