//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Core/Macros.hpp"

#define STBI_ASSERT(x)  MGN_CORE_CASSERT(x)
#define STBIR_ASSERT(x) MGN_CORE_CASSERT(x)
#define STBIW_ASSERT(x) MGN_CORE_CASSERT(x)
#define STBTT_assert(x) MGN_CORE_CASSERT(x)

#define STB_IMAGE_IMPLEMENTATION 1
#define STB_IMAGE_RESIZE_IMPLEMENTATION 1
#define STB_IMAGE_WRITE_IMPLEMENTATION 1
#define STB_PERLIN_IMPLEMENTATION 1
#define STB_SPRINTF_IMPLEMENTATION 1
#define STB_TRUETYPE_IMPLEMENTATION 1

#include <stb_image.h>
#include <stb_image_resize2.h>
#include <stb_image_write.h>
#include <stb_perlin.h>
#include <stb_sprintf.h>
#include <stb_truetype.h>

#include "Imagine/ThirdParty/Stb.hpp"

namespace Imagine::ThirdParty::Stb {

	namespace Image {

		Imagine::Core::Image<uint8_t> Load(const char *path, int desired_channels /*= 0*/) {
			int width, height, channels;
			stbi_uc *image = stbi_load(path, &width, &height, &channels, desired_channels);
			if (image) {
				return Core::Image<uint8_t>{image, (uint32_t) width, (uint32_t) height, (uint32_t) channels};
			}
			else {
				return Core::Image<uint8_t>{};
			}
		}

		Imagine::Core::Image<uint16_t> Load16(const char *path, int desired_channels) {
			int width, height, channels;
			stbi_us *image = stbi_load_16(path, &width, &height, &channels, desired_channels);
			if (image) {
				return Core::Image<uint16_t>{image, (uint32_t) width, (uint32_t) height, (uint32_t) channels};
			}
			else {
				return Core::Image<uint16_t>{};
			}
		}

		Imagine::Core::Image<float> LoadFloat(const char *path, int desired_channels) {
			int width, height, channels;
			float *image = stbi_loadf(path, &width, &height, &channels, desired_channels);
			if (image) {
				return Core::Image<float>{image, (uint32_t) width, (uint32_t) height, (uint32_t) channels};
			}
			else {
				return Core::Image<float>{};
			}
		}

		Core::Image<uint8_t> LoadFromMemory(Core::ConstBufferView memoryImage, int desired_channels /*= 0*/) {
			int width, height, channels;
			stbi_uc *image = stbi_load_from_memory(memoryImage.Get<stbi_uc>(), memoryImage.Size(), &width, &height, &channels, desired_channels);
			if (image) {
				return Core::Image<uint8_t>{image, (uint32_t) width, (uint32_t) height, (uint32_t) channels};
			}
			else {
				return Core::Image<uint8_t>{};
			}
		}

		Core::Image<uint16_t> Load16FromMemory(Core::ConstBufferView memoryImage, int desired_channels /*= 0*/) {
			int width, height, channels;
			stbi_us *image = stbi_load_16_from_memory(memoryImage.Get<stbi_uc>(), memoryImage.Size(), &width, &height, &channels, desired_channels);
			if (image) {
				return Core::Image<uint16_t>{image, (uint32_t) width, (uint32_t) height, (uint32_t) channels};
			}
			else {
				return Core::Image<uint16_t>{};
			}
		}

		Imagine::Core::Image<float> LoadFloatFromMemory(Core::ConstBufferView memoryImage, int desired_channels) {
			int width, height, channels;
			float *image = stbi_loadf_from_memory(memoryImage.Get<stbi_uc>(), memoryImage.Size(), &width, &height, &channels, desired_channels);
			if (image) {
				return Core::Image<float>{image, (uint32_t) width, (uint32_t) height, (uint32_t) channels};
			}
			else {
				return Core::Image<float>{};
			}
		}
	} // namespace Image

	namespace Perlin {

		float Noise3(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap) {
			return stb_perlin_noise3(x, y, z, x_wrap, y_wrap, z_wrap);
		}
		float Noise3Seed(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap, int seed) {
			return stb_perlin_noise3_seed(x, y, z, x_wrap, y_wrap, z_wrap, seed);
		}
		float RidgeNoise3(float x, float y, float z, float lacunarity, float gain, float offset, int octaves) {
			return stb_perlin_ridge_noise3(x, y, z, lacunarity, gain, offset, octaves);
		}
		float FbmNoise3(float x, float y, float z, float lacunarity, float gain, int octaves) {
			return stb_perlin_fbm_noise3(x, y, z, lacunarity, gain, octaves);
		}
		float TurbulenceNoise3(float x, float y, float z, float lacunarity, float gain, int octaves) {
			return stb_perlin_turbulence_noise3(x, y, z, lacunarity, gain, octaves);
		}
		float Noise3WrapNonpow2(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap, unsigned char seed) {
			return stb_perlin_noise3_wrap_nonpow2(x, y, z, x_wrap, y_wrap, z_wrap, seed);
		}

	} // namespace Perlin

} // namespace Imagine::ThirdParty::Stb
