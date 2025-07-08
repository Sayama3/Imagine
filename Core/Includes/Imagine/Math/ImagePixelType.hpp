//
// Created by ianpo on 08/07/2025.
//

#pragma once

namespace Imagine::Core {

	enum class ImagePixelType : uint8_t {
		None = 0,

		Uint8,
		Uint16,
		Uint32,

		Int8,
		Int16,
		Int32,

		F32,
	};

	static inline constexpr std::string ImagePixelTypeToString(const ImagePixelType type) {
		switch (type) {
			case ImagePixelType::None:
				return "None";
				break;
			case ImagePixelType::Uint8:
				return "Uint8";
				break;
			case ImagePixelType::Uint16:
				return "Uint16";
				break;
			case ImagePixelType::Uint32:
				return "Uint32";
				break;
			case ImagePixelType::Int8:
				return "Int8";
				break;
			case ImagePixelType::Int16:
				return "Int16";
				break;
			case ImagePixelType::Int32:
				return "Int32";
				break;
			case ImagePixelType::F32:
				return "F32";
				break;
		}
		return "Unknown";
	}

	static inline constexpr bool TryImagePixelTypeFromString(const std::string& str, ImagePixelType& type) {
		if(str == "None") {type = ImagePixelType::None; return true;}
		if(str == "Uint8") {type = ImagePixelType::Uint8; return true;}
		if(str == "Uint16") {type = ImagePixelType::Uint16; return true;}
		if(str == "Uint32") {type = ImagePixelType::Uint32; return true;}
		if(str == "Int8") {type = ImagePixelType::Int8; return true;}
		if(str == "Int16") {type = ImagePixelType::Int16; return true;}
		if(str == "Int32") {type = ImagePixelType::Int32; return true;}
		if(str == "F32") {type = ImagePixelType::F32; return true;}
		return false;
	}

	static inline constexpr ImagePixelType GetImagePixelTypeFromString(const std::string& str) {
		ImagePixelType pt = ImagePixelType::None;
		TryImagePixelTypeFromString(str, pt);
		return pt;
	}


	template<typename PixelType> inline static constexpr ImagePixelType GetPixelType() {return ImagePixelType::None;}
	template<> inline static constexpr ImagePixelType GetPixelType<uint8_t>() {return ImagePixelType::Uint8;}
	template<> inline static constexpr ImagePixelType GetPixelType<uint16_t>() {return ImagePixelType::Uint16;}
	template<> inline static constexpr ImagePixelType GetPixelType<uint32_t>() {return ImagePixelType::Uint32;}
	template<> inline static constexpr ImagePixelType GetPixelType<int8_t>() {return ImagePixelType::Int8;}
	template<> inline static constexpr ImagePixelType GetPixelType<int16_t>() {return ImagePixelType::Int16;}
	template<> inline static constexpr ImagePixelType GetPixelType<int32_t>() {return ImagePixelType::Int32;}
	template<> inline static constexpr ImagePixelType GetPixelType<float>() {return ImagePixelType::F32;}
}