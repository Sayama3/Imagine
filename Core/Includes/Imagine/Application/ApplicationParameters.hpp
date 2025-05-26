//
// Created by Sayama on 29/04/2025.
//

#pragma once
#include "WindowParameters.hpp"

namespace Imagine::Core {

	struct ApplicationParameters {
		static inline constexpr std::string EngineName{"Imagine"};
		std::string AppName;
		std::optional<WindowParameters> Window;
		bool UseRenderer;
	};
} // Imagine::Core