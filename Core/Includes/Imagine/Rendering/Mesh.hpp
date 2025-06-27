//
// Created by Iannis on 17/06/2025.
//

#pragma once
#include "Imagine/Rendering/Material.hpp"

namespace Imagine::Core {

	struct Mesh {
		Mesh() = default;
		virtual ~Mesh() {};

		struct LOD {
			uint32_t index;
			uint32_t count;
			std::shared_ptr<MaterialInstance> material;
		};

		std::string name;
		std::vector<LOD> lods{};
	};

} // namespace Imagine::Core
