//
// Created by ianpo on 08/07/2025.
//

#pragma once

#include "Imagine/Rendering/CPU/CPUVirtualTexture.hpp"
#include "Imagine/Rendering/TextureParameters.hpp"
#include "YAML_DEFINE.hpp"

namespace YAML {
	YAML_SIMPLE_MGN_ENUM_EMITTER(ImagePixelType);
	YAML_SIMPLE_MGN_ENUM_EMITTER(TextureUsage);

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::VirtualTexture2D &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Usage", v.usage);
			out << KEYVAL("Type", v.type);
			out << KEYVAL("Channels", v.channels);
			out << KEYVAL("Width", v.width);
			out << KEYVAL("Height", v.height);
		}
		out << YAML::EndMap;
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::VirtualTexture3D &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Usage", v.usage);
			out << KEYVAL("Type", v.type);
			out << KEYVAL("Channels", v.channels);
			out << KEYVAL("Width", v.width);
			out << KEYVAL("Height", v.height);
			out << KEYVAL("Depth", v.depth);
		}
		out << YAML::EndMap;
		return out;
	}

} // namespace YAML

namespace YAML {
	YAML_SIMPLE_MGN_ENUM_CONVERTER(ImagePixelType);
	YAML_SIMPLE_MGN_ENUM_CONVERTER(TextureUsage);

	template<>
	struct convert<Imagine::Core::VirtualTexture2D> {
		inline static Node encode(const Imagine::Core::VirtualTexture2D &v) {
			Node node;
			node["Usage"] = v.usage;
			node["Type"] = v.type;
			node["Channels"] = v.channels;
			node["Width"] = v.width;
			node["Height"] = v.height;
			return node;
		}
		inline static bool decode(const Node &node, Imagine::Core::VirtualTexture2D &v) {
			v.usage = node["Usage"].as<Imagine::Core::TextureUsage>();
			v.type = node["Type"].as<Imagine::Core::ImagePixelType>();
			v.channels = node["Channels"].as<uint32_t>();
			v.width = node["Width"].as<uint32_t>();
			v.height = node["Height"].as<uint32_t>();
			return true;
		}
	};

	template<>
	struct convert<Imagine::Core::VirtualTexture3D> {
		inline static Node encode(const Imagine::Core::VirtualTexture3D &v) {
			Node node;
			node["Usage"] = v.usage;
			node["Type"] = v.type;
			node["Channels"] = v.channels;
			node["Width"] = v.width;
			node["Height"] = v.height;
			node["Depth"] = v.depth;
			return node;
		}
		inline static bool decode(const Node &node, Imagine::Core::VirtualTexture3D &v) {
			v.usage = node["Usage"].as<Imagine::Core::TextureUsage>();
			v.type = node["Type"].as<Imagine::Core::ImagePixelType>();
			v.channels = node["Channels"].as<uint32_t>();
			v.width = node["Width"].as<uint32_t>();
			v.height = node["Height"].as<uint32_t>();
			v.depth = node["Depth"].as<uint32_t>();
			return true;
		}
	};
	;
} // namespace YAML
