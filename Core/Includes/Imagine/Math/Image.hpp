//
// Created by ianpo on 12/05/2024.
//

#pragma once

#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"

namespace Imagine::Core
{
	template<typename PixelType = uint8_t>
	class Image {
	public:
		static inline constexpr uint32_t PixelSize = sizeof(PixelType);
	public:
		Image() = default;
		Image(PixelType* source, uint32_t w, uint32_t h, uint32_t c);
		Image(uint32_t w, uint32_t h, uint32_t c);
		Image(const Image& image);
		Image(Image&& image) noexcept;
		~Image();

		Image& operator=(Image&& other) noexcept;
		void Reset(PixelType* source, uint32_t w, uint32_t h, uint32_t c);

		void Allocate(uint32_t w, uint32_t h, uint32_t c);
		void Release();

		void Zeroes();

		BufferView operator()(uint32_t x, uint32_t y);
		ConstBufferView operator()(uint32_t x, uint32_t y) const;

		PixelType& operator()(uint32_t x, uint32_t y, uint32_t c);
		const PixelType& operator()(uint32_t x, uint32_t y, uint32_t c) const;

		[[nodiscard]] Image Splice(uint32_t offsetX, uint32_t offsetY, uint32_t newWidth, uint32_t newHeight) const;
	public:
		Buffer source = Buffer();
		uint32_t width = 0, height = 0, channels = 0;
	};


	template<typename PixelType>
	inline void Image<PixelType>::Zeroes() {
		source.Zeroes();
	}

	template<typename PixelType>
	inline void Image<PixelType>::Release() {
		source.Release();
		width = 0;
		height = 0;
		channels = 0;
	}

	template<typename PixelType>
	BufferView Image<PixelType>::operator()(const uint32_t x, const uint32_t y) {
		return BufferView{source, y * width * channels * PixelSize + x * channels * PixelSize, sizeof(PixelType) * channels};
	}

	template<typename PixelType>
	ConstBufferView Image<PixelType>::operator()(const uint32_t x, const uint32_t y) const {
		return ConstBufferView{source.Get(), y * width * channels * PixelSize + x * channels * PixelSize, sizeof(PixelType) * channels};
	}

	template<typename PixelType>
	inline void Image<PixelType>::Allocate(const uint32_t w, const uint32_t h, const uint32_t c)
	{
		source.Reallocate(w * h * c * PixelSize);
		width = w;
		height = h;
		channels = c;
	}

	template<typename PixelType>
	inline const PixelType &Image<PixelType>::operator()(const uint32_t x, const uint32_t y, const uint32_t c) const
	{
		return source.Get<PixelType>(y * width * channels * PixelSize + x * channels * PixelSize + c * PixelSize);
	}

	template<typename PixelType>
	inline PixelType &Image<PixelType>::operator()(const uint32_t x, const uint32_t y, const uint32_t c)
	{
		return source.Get<PixelType>(y * width * channels * PixelSize + x * channels * PixelSize + c * PixelSize);
	}

	template<typename PixelType>
	inline void Image<PixelType>::Reset(PixelType* s, const uint32_t w, const uint32_t h, const uint32_t c)
	{
		source.Release();
		source = Buffer{s, w * h * c * PixelSize};
		width = w;
		height = h;
		channels = c;
	}

	template<typename PixelType>
	inline void Image<PixelType>::Reset(const uint32_t w, const uint32_t h, const uint32_t c)
	{
		source.Reallocate(w * h * c * PixelSize);
		width = w;
		height = h;
		channels = c;
	}

	template<typename PixelType>
	inline Image<PixelType> & Image<PixelType>::operator=(Image&& other) noexcept 	{
		// Release previous image
		source.Release();

		// Take the image from the other
		source.swap(other.source);
		width = other.width;
		height = other.height;
		channels = other.channels;

		// Clear the other
		other.width = 0;
		other.height = 0;
		other.channels = 0;

		return *this;
	}

	template<typename PixelType>
	Image<PixelType>::~Image()
	{
		Release();
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(uint32_t w, uint32_t h, uint32_t c)
			: source(w*h*c*PixelSize), width(w), height(h), channels(c)
	{
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(PixelType* s, uint32_t w, uint32_t h, uint32_t c)
			: source(s, w * h * c * PixelSize), width(w), height(h), channels(c)
	{
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(const Image& other) : source(Buffer::Copy(other.source)), width(other.width), height(other.height), channels(other.channels)
	{
	}

	template<typename PixelType>
	inline Image<PixelType>::Image(Image&& other) noexcept : source(std::move(other.source)), width(other.width), height(other.height), channels(other.channels)
	{
		other.source.Release();
		other.width = 0;
		other.height = 0;
		other.channels = 0;
	}

	template<typename PixelType>
	inline Image<PixelType> Image<PixelType>::Splice(uint32_t offsetX, uint32_t offsetY, uint32_t newWidth, uint32_t newHeight) const
	{
		// MGN_PROFILE_FUNCTION();
		Image result(newWidth, newHeight, channels);

		for (uint32_t x = 0; x < newWidth; ++x) {
			auto offX = offsetX + x;
			for (uint32_t y = 0; y < newHeight; ++y) {
				auto offY = offsetY + y;
				for (uint32_t c = 0; c < channels; ++c) {
					result(x,y,c) = operator()(offX, offY, c);
				}
			}
		}

		return result;
	}

} // namespace Imagine::Core

