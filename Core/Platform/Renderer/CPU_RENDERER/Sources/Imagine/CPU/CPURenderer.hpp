//
// Created by Sayama on 23/05/2025.
//

#pragma once

#include "Imagine/Rendering/Renderer.hpp"

namespace Imagine::CPU {
	class CPURenderer final : public Core::Renderer {
public:
		CPURenderer();
		CPURenderer(const RendererParameter& renderParams, const ApplicationParameters& appParams);
		virtual ~CPURenderer() override;
	};
}