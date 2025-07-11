//
// Created by Iannis on 02/05/2025.
//

#pragma once

#include "Imagine/Core/HeapArray.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine {
	struct EntityID {
		static inline constexpr uint32_t NullID{-1u};

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
		Vec3 LocalPosition{0};
		Quat LocalRotation{Math::Identity<Quat>()};
		Vec3 LocalScale{1};

		Mat4 GetLocalMatrix() const;
	};
} // namespace Imagine



template<>
struct std::hash<Imagine::EntityID> {
	std::size_t operator()(const Imagine::EntityID &s) const noexcept {
		return std::hash<uint32_t>{}(s.id);
	}
};

inline std::ostream &operator<<(std::ostream &os, const Imagine::EntityID &value) {
	return os << value.string();
}