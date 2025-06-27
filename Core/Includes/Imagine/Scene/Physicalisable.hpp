//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core {

	struct Physicalisable {
		Real Mass;
		Vec3 Velocity;
		Vec3 AngularVelocity;
		UUID IdToPhysics;
	};

} // namespace Imagine::Core
