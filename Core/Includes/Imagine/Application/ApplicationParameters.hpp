//
// Created by Sayama on 29/04/2025.
//

#pragma once

#include "Imagine/Application/WindowParameters.hpp"
#include "Imagine/Core/Macros.hpp"
#include "Imagine/Rendering/RendererParameters.hpp"

namespace Imagine::Core {

	struct ApplicationParameters {
		static inline constexpr const char* const EngineName{"Imagine"};
		std::string AppName;
		uint32_t AppVersion;
		std::optional<WindowParameters> Window;
		std::optional<RendererParameters> Renderer;

		[[nodiscard]] uint32_t GetMajor() const;
		[[nodiscard]] uint32_t GetMinor() const;
		[[nodiscard]] uint32_t GetPatch() const;
		[[nodiscard]] std::string GetVersionStr() const;
		void SetVersion(uint32_t major, uint32_t minor, uint32_t patch);
	};
} // Imagine::Core
