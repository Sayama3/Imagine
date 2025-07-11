//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Math/Image.hpp"

namespace Imagine::ThirdParty {
	namespace Stb {
		namespace Image {
			Imagine::Image<uint8_t> Load(const char *path, int desired_channels = 0);
			Imagine::Image<uint16_t> Load16(const char *path, int desired_channels = 0);
			Imagine::Image<float> LoadFloat(const char *path, int desired_channels = 0);
			Imagine::Image<uint8_t> LoadFromMemory(ConstBufferView memoryImage, int desired_channels = 0);
			Imagine::Image<uint16_t> Load16FromMemory(ConstBufferView memoryImage, int desired_channels = 0);
			Imagine::Image<float> LoadFloatFromMemory(ConstBufferView memoryImage, int desired_channels = 0);
		} // namespace Image
		namespace Perlin {
			float Noise3(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap);
			float Noise3Seed(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap, int seed);
			float RidgeNoise3(float x, float y, float z, float lacunarity, float gain, float offset, int octaves);
			float FbmNoise3(float x, float y, float z, float lacunarity, float gain, int octaves);
			float TurbulenceNoise3(float x, float y, float z, float lacunarity, float gain, int octaves);
			float Noise3WrapNonpow2(float x, float y, float z, int x_wrap, int y_wrap, int z_wrap, unsigned char seed);
		} // namespace Perlin
	} // namespace Stb

} // namespace Imagine::ThirdParty
