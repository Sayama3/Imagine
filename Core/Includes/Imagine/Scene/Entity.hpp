//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/HeapArray.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core {
	struct EntityID {
		static inline constexpr EntityID NullID{-1u};

		EntityID() = default;
		EntityID(const uint32_t id) : id(id) {}
		~EntityID() = default;
		EntityID(const EntityID&) = default;
		EntityID& operator=(const EntityID&) = default;

		uint32_t id{-1u};

		[[nodiscard]] bool IsValid() const;
		[[nodiscard]] UUID AsUUID() const { return UUID(id); }
		[[nodiscard]] std::string string() const { return std::to_string(id); }

		bool operator==(const EntityID & o) const {return id == o.id;}
		bool operator!=(const EntityID & o) const {return id != o.id;}
		auto operator<=>(const EntityID & o) const {return id <=> o.id;}
	};

	struct Entity {
		EntityID Id;
		Quat Rotation;
		Vec3 Position;
	};
} // namespace Imagine::Core
