//
// Created by ianpo on 27/05/2025.
//

#pragma once


namespace Imagine::Core {

#ifdef MGN_DEBUG
	inline static constexpr bool c_DefaultDebugRendering = true;
#else
	inline static constexpr bool c_DefaultDebugRendering = false;
#endif

	struct RendererParameters {
		uint16_t NbrFrameInFlight = 2;
		bool EnableDebug = c_DefaultDebugRendering;
	};
} // namespace Imagine::Core
