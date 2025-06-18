//
// Created by Iannis on 02/05/2025.
//

#include "Imagine/Scene/Entity.hpp"

namespace Imagine::Core {
	bool EntityID::IsValid() const {
		return id != -1u;
	}
	Mat4 Entity::GetLocalMatrix() const {
		return Math::TRS(LocalPosition, LocalRotation, LocalScale);
	}
} // namespace Imagine::Core
