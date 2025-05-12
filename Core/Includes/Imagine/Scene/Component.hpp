//
// Created by ianpo on 12/05/2025.
//

#pragma once

#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/UUID.hpp"

namespace Imagine::Core {

	struct ComponentID {
		uint64_t id;
		[[nodiscard]] UUID AsUUID() const {return UUID(id);}
	};

	struct Component
	{
		Component() : Buffer{}, ComponentSize{0} {}
		Component(Buffer&& buffer, const uint64_t componentSize) : Buffer(std::move(buffer)), ComponentSize(componentSize) {}
		Component(const uint64_t count, const uint64_t componentSize) : Buffer(componentSize*count), ComponentSize(componentSize) {}
		template<typename T>
		explicit Component(const uint64_t count) : Buffer(sizeof(T)*count), ComponentSize(sizeof(T)) {}

		Component(const Component&) = delete;
		Component& operator=(const Component&) = delete;

		Component(Component&& other) noexcept {swap(other);}
		Component& operator=(Component&& other) noexcept {swap(other);return *this;}

		~Component() = default;

		void swap(Component& other) noexcept {std::swap(Buffer, other.Buffer);std::swap(ComponentSize, other.ComponentSize);}

		Buffer Buffer;
		uint64_t ComponentSize;
	};

}
