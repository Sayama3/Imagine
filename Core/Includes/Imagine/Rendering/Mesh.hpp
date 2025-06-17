//
// Created by Iannis on 17/06/2025.
//

#pragma once

namespace Imagine::Core {

	struct Mesh {
		Mesh() = default;
		virtual ~Mesh() {};

		struct LOD {
			uint32_t index;
			uint32_t count;
		};

		std::vector<LOD> lods{};
	};

} // namespace Imagine::Core
