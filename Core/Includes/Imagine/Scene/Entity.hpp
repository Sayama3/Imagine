//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core {
	struct EntityID {
		uint32_t id;
		[[nodiscard]] UUID AsUUID() const { return UUID(id); }
		[[nodiscard]] std::string string() const { return std::to_string(id); }
	};

	struct Entity {
		EntityID Id;
		Quat Rotation;
		Vec3 Position;
	};
} // namespace Imagine::Core
