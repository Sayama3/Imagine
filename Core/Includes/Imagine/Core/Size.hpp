//
// Created by ianpo on 27/05/2025.
//

#pragma once

namespace Imagine {
	struct Size2 {
		Size2() :
			x(0), y(0) {}
		Size2(const uint32_t x, const uint32_t y) :
			x(x), y(y) {}
		Size2(const Size2 &other) :
			x(other.x), y(other.y) {}
		Size2 &operator=(const Size2 &other) {
			x = other.x;
			y = other.y;
			return *this;
		}
		~Size2() = default;

		uint32_t &operator[](const uint32_t index) { return size[index]; }
		uint32_t operator[](const uint32_t index) const { return size[index]; }

		[[nodiscard]] bool operator==(const Size2 & size2) const { return x ==size2.x && y == size2.y; }
		[[nodiscard]] bool operator!=(const Size2 &size2) const { return !(*this == size2); }

		union {
			struct {
				uint32_t x, y;
			};
			struct {
				uint32_t width, height;
			};
			uint32_t size[2];
		};
	};

	struct Size3 {
		Size3() :
			x(0), y(0), z(0) {}
		Size3(const uint32_t x, const uint32_t y, const uint32_t z) :
			x(x), y(y), z(z) {}
		Size3(const Size3 &other) :
			x(other.x), y(other.y), z(other.z) {}
		Size3 &operator=(const Size3 &other) {
			x = other.x;
			y = other.y;
			z = other.z;
			return *this;
		}
		~Size3() = default;

		uint32_t &operator[](const uint32_t index) { return size[index]; }
		uint32_t operator[](const uint32_t index) const { return size[index]; }

		union {
			struct {
				uint32_t x, y, z;
			};
			struct {
				uint32_t width, height, depth;
			};
			uint32_t size[3];
		};
	};
} // namespace Imagine
