//
// Created by Iannis on 18/06/2025.
//

#include "Imagine/Core/Math.hpp"

namespace Imagine {
	Vec2 Rect::GetCenter() const {
		return (max + min) * 0.5f;
	}
	Vec2 Rect::GetSize() const {
		return max - min;
	}
	Vec2 Rect::GetRelative(Vec2 globalPos) const {
		return (globalPos - min) / GetSize();
	}
	Vec2 Rect::GetGlobal(Vec2 relativePos) const {
		return (relativePos * GetSize()) + min;
	}
	bool Rect::IsInside(Vec2 globalPos) const {
		return globalPos.x >= min.x &&
			   globalPos.y >= min.y &&
			   globalPos.x <= max.x &&
			   globalPos.y <= max.y;
	}
} // namespace Imagine
