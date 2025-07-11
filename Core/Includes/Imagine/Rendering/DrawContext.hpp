//
// Created by Iannis on 16/06/2025.
//

#pragma once
#include "Imagine/Rendering/RenderObject.hpp"


namespace Imagine {

	class DrawContext {
	public:
		std::vector<RenderObject> OpaqueSurfaces;
		std::vector<LineObject> OpaqueLines;
		std::vector<PointObject> OpaquePoints;

		void Clear() {
			OpaqueSurfaces.clear();
			OpaqueLines.clear();
			OpaquePoints.clear();
		}
	};

} // namespace Imagine
