//
// Created by ianpo on 01/06/2025.
//

#pragma once

namespace Imagine::Core {

	enum class MaterialPass : uint8_t {
		MainColor,
		Transparent,
		Other
	};

	struct MaterialInstance {
		MaterialInstance() = default;
		virtual ~MaterialInstance() = default;

		MaterialPass passType;
	};

} // namespace Imagine::Core
