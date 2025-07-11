//
// Created by ianpo on 12/05/2025.
//

#pragma once

#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine {

	struct ComponentID {
		uint64_t id;
		[[nodiscard]] UUID AsUUID() const { return UUID(id); }
	};

	struct Component {
		Component() :
			buffer{}, componentSize{0} {}
		Component(Buffer &&buffer, const uint64_t componentSize) :
			buffer(std::move(buffer)), componentSize(componentSize) {}
		Component(const uint64_t count, const uint64_t componentSize) :
			buffer(componentSize * count), componentSize(componentSize) {}
		template<typename T>
		explicit Component(const uint64_t count) :
			buffer(sizeof(T) * count), componentSize(sizeof(T)) {}

		Component(const Component &) = delete;
		Component &operator=(const Component &) = delete;

		Component(Component &&other) noexcept { swap(other); }
		Component &operator=(Component &&other) noexcept {
			swap(other);
			return *this;
		}

		~Component() = default;

		void swap(Component &other) noexcept {
			std::swap(buffer, other.buffer);
			std::swap(componentSize, other.componentSize);
		}

		Buffer buffer;
		uint64_t componentSize;
	};

} // namespace Imagine
