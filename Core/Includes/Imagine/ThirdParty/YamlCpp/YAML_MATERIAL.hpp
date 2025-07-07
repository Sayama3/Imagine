//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include "../YAML_SHADER.hpp"
#include "../YAML_STD.hpp"
#include "YAML_DEFINE.hpp"
#include "YAML_MESH.hpp"

#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Rendering/MaterialComponents.hpp"

namespace YAML {

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialFilling &v) {
		out << Imagine::Core::Helper::MaterialFillingToString(v);
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialPass &v) {
		out << Imagine::Core::Helper::MaterialPassToString(v);
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialType &v) {
		out << Imagine::Core::Helper::MaterialTypeToString(v);
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialField &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Name", v.name);
			out << KEYVAL("Type", v.type);
			out << KEYVAL("Data", v.data);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialBlock::BufferType &v) {
		switch (v) {
			case Imagine::Core::MaterialBlock::SSBO:
				out << "SSBO";
				break;
			case Imagine::Core::MaterialBlock::Uniform:
				out << "Uniform";
				break;
			default:
				out << "Unknown";
		}
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialBlock &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Name", v.Name);
			out << KEYVAL("Read", v.Read);
			out << KEYVAL("Write", v.Write);
			out << KEYVAL("GPUBufferType", v.GPUBufferType);
			out << KEYVAL("Fields", v.Fields);
			out << v.Fields;
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialPushConstant &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Block", v.Block);
			out << KEYVAL("Stages", v.Stages);
			out << KEYVAL("Offset", v.offset);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialSet &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Blocks", v.Blocks);
			out << KEYVAL("Stages", v.Stages);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::MaterialLayout &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Sets", v.Sets);
			out << KEYVAL("PushConstants", v.PushConstants);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::CPUMaterial &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Handle", v.Handle);
			out << KEYVAL("Layout", v.layout);
			out << KEYVAL("Shaders", v.shaders);
			out << KEYVAL("Pass", v.pass);
			out << KEYVAL("Topology", v.topology);
			out << KEYVAL("Mode", v.mode);
		}
		out << YAML::EndMap;
		return out;
	}
} // namespace Imagine::ThirdParty::YamlCpp


namespace YAML {
	template<>
	struct convert<::Imagine::Core::MaterialFilling> {
		inline static Node encode(const ::Imagine::Core::MaterialFilling &rhs) {
			Node node;
			node = Imagine::Core::Helper::MaterialFillingToString(rhs);
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialFilling &rhs) {
			return Imagine::Core::Helper::TryMaterialFillingFromString(node.as<std::string>(), rhs);
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialPass> {
		inline static Node encode(const ::Imagine::Core::MaterialPass &rhs) {
			Node node;
			node = Imagine::Core::Helper::MaterialPassToString(rhs);
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialPass &rhs) {
			return Imagine::Core::Helper::TryMaterialPassFromString(node.as<std::string>(), rhs);
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialType> {
		inline static Node encode(const ::Imagine::Core::MaterialType &rhs) {
			Node node;
			node = Imagine::Core::Helper::MaterialTypeToString(rhs);
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialType &rhs) {
			return Imagine::Core::Helper::TryMaterialTypeFromString(node.as<std::string>(), rhs);
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialField> {
		inline static Node encode(const ::Imagine::Core::MaterialField &rhs) {
			Node node;
			node["Name"] = rhs.name;
			node["Type"] = rhs.type;
			node["Data"] = rhs.data;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialField &rhs) {
			rhs.name = node["Name"].as<std::string>();
			rhs.type = node["Type"].as<Imagine::Core::MaterialType>();
			rhs.data = node["Data"].as<std::array<uint8_t, ::Imagine::Core::MaterialField::DataSize>>();
			return false;
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialBlock::BufferType> {
		inline static Node encode(const ::Imagine::Core::MaterialBlock::BufferType &rhs) {
			Node node;
			switch (rhs) {
				case Imagine::Core::MaterialBlock::SSBO:
					node = "SSBO";
					break;
				case Imagine::Core::MaterialBlock::Uniform:
					node = "Uniform";
					break;
			}
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialBlock::BufferType &rhs) {
			std::string str = node.as<std::string>();
			if (str == "Uniform") {
				rhs = ::Imagine::Core::MaterialBlock::BufferType::Uniform;
				return true;
			}
			if (str == "SSBO") {
				rhs = ::Imagine::Core::MaterialBlock::BufferType::SSBO;
				return true;
			}
			return false;
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialBlock> {
		inline static Node encode(const ::Imagine::Core::MaterialBlock &rhs) {
			Node node;
			node["Fields"] = rhs.Fields;
			node["Name"] = rhs.Name;
			node["Read"] = rhs.Read;
			node["Write"] = rhs.Write;
			node["GPUBufferType"] = rhs.GPUBufferType;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialBlock &rhs) {
			rhs.Fields = node["Fields"].as<std::vector<::Imagine::Core::MaterialField>>();
			rhs.Name = node["Name"].as<std::string>();
			rhs.Read = node["Read"].as<bool>();
			rhs.Write = node["Write"].as<bool>();
			rhs.GPUBufferType = node["GPUBufferType"].as<Imagine::Core::MaterialBlock::BufferType>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialSet> {
		inline static Node encode(const ::Imagine::Core::MaterialSet &rhs) {
			Node node;
			node["Blocks"] = rhs.Blocks;
			node["Stages"] = rhs.Stages;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialSet &rhs) {
			rhs.Blocks = node["Blocks"].as<std::vector<Imagine::Core::MaterialBlock>>();
			rhs.Stages = node["Stages"].as<Imagine::Core::ShaderStage>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialPushConstant> {
		inline static Node encode(const ::Imagine::Core::MaterialPushConstant &rhs) {
			Node node;
			node["Block"] = rhs.Block;
			node["Stages"] = rhs.Stages;
			node["Offset"] = rhs.offset;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialPushConstant &rhs) {
			rhs.Block = node["Block"].as<Imagine::Core::MaterialBlock>();
			rhs.Stages = node["Stages"].as<Imagine::Core::ShaderStage>();
			rhs.offset = node["Offset"].as<uint32_t>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::MaterialLayout> {
		inline static Node encode(const ::Imagine::Core::MaterialLayout &rhs) {
			Node node;
			node["Sets"] = rhs.Sets;
			node["PushConstants"] = rhs.PushConstants;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::MaterialLayout &rhs) {
			rhs.Sets = node["Sets"].as<std::vector<Imagine::Core::MaterialSet>>();
			rhs.PushConstants = node["PushConstants"].as<std::vector<Imagine::Core::MaterialPushConstant>>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::CPUMaterial> {
		inline static Node encode(const ::Imagine::Core::CPUMaterial &v) {
			Node node;
			node["Handle"] = v.Handle;
			node["Layout"] = v.layout;
			node["Shaders"] = v.shaders;
			node["Pass"] = v.pass;
			node["Topology"] = v.topology;
			node["Mode"] = v.mode;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::CPUMaterial &v) {
			v.Handle = node["Handle"].as<Imagine::Core::AssetHandle>();
			v.layout = node["Layout"].as<Imagine::Core::MaterialLayout>();
			v.shaders = node["Shaders"].as<std::array<Imagine::Core::AssetHandle, 5>>();
			v.pass = node["Pass"].as<Imagine::Core::MaterialPass>();
			v.topology = node["Topology"].as<Imagine::Core::Topology>();
			v.mode = node["Mode"].as<Imagine::Core::MaterialFilling>();
			return true;
		}
	};

} // namespace YAML
