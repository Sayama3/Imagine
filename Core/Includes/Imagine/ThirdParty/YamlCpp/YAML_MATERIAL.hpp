//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include "YAML_SHADER.hpp"
#include "YAML_STD.hpp"
#include "YAML_DEFINE.hpp"
#include "YAML_MESH.hpp"

#include "Imagine/Rendering/CPU/CPUMaterial.hpp"
#include "Imagine/Rendering/MaterialComponents.hpp"

namespace YAML {

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialFilling &v) {
		out << Imagine::Helper::MaterialFillingToString(v);
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialPass &v) {
		out << Imagine::Helper::MaterialPassToString(v);
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialType &v) {
		out << Imagine::Helper::MaterialTypeToString(v);
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialField &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Name", v.name);
			out << KEYVAL("Type", v.type);
			out << KEYVAL("Data", v.data);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialBlock::BufferType &v) {
		switch (v) {
			case Imagine::MaterialBlock::SSBO:
				out << "SSBO";
				break;
			case Imagine::MaterialBlock::Uniform:
				out << "Uniform";
				break;
			default:
				out << "Unknown";
		}
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialBlock &v) {
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

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialPushConstant &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Block", v.Block);
			out << KEYVAL("Stages", v.Stages);
			out << KEYVAL("Offset", v.offset);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialSet &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Blocks", v.Blocks);
			out << KEYVAL("Stages", v.Stages);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::MaterialLayout &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Sets", v.Sets);
			out << KEYVAL("PushConstants", v.PushConstants);
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUMaterial &v) {
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
	struct convert<::Imagine::MaterialFilling> {
		inline static Node encode(const ::Imagine::MaterialFilling &v) {
			Node node;
			node = Imagine::Helper::MaterialFillingToString(v);
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialFilling &v) {
			return Imagine::Helper::TryMaterialFillingFromString(node.as<std::string>(), v);
		}
	};

	template<>
	struct convert<::Imagine::MaterialPass> {
		inline static Node encode(const ::Imagine::MaterialPass &v) {
			Node node;
			node = Imagine::Helper::MaterialPassToString(v);
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialPass &v) {
			return Imagine::Helper::TryMaterialPassFromString(node.as<std::string>(), v);
		}
	};

	template<>
	struct convert<::Imagine::MaterialType> {
		inline static Node encode(const ::Imagine::MaterialType &v) {
			Node node;
			node = Imagine::Helper::MaterialTypeToString(v);
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialType &v) {
			return Imagine::Helper::TryMaterialTypeFromString(node.as<std::string>(), v);
		}
	};

	template<>
	struct convert<::Imagine::MaterialField> {
		inline static Node encode(const ::Imagine::MaterialField &v) {
			Node node;
			node["Name"] = v.name;
			node["Type"] = v.type;
			node["Data"] = v.data;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialField &v) {
			v.name = node["Name"].as<std::string>();
			v.type = node["Type"].as<Imagine::MaterialType>();
			v.data = node["Data"].as<std::array<uint8_t, ::Imagine::MaterialField::DataSize>>();
			return false;
		}
	};

	template<>
	struct convert<::Imagine::MaterialBlock::BufferType> {
		inline static Node encode(const ::Imagine::MaterialBlock::BufferType &v) {
			Node node;
			switch (v) {
				case Imagine::MaterialBlock::SSBO:
					node = "SSBO";
					break;
				case Imagine::MaterialBlock::Uniform:
					node = "Uniform";
					break;
			}
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialBlock::BufferType &v) {
			std::string str = node.as<std::string>();
			if (str == "Uniform") {
				v = ::Imagine::MaterialBlock::BufferType::Uniform;
				return true;
			}
			if (str == "SSBO") {
				v = ::Imagine::MaterialBlock::BufferType::SSBO;
				return true;
			}
			return false;
		}
	};

	template<>
	struct convert<::Imagine::MaterialBlock> {
		inline static Node encode(const ::Imagine::MaterialBlock &v) {
			Node node;
			node["Fields"] = v.Fields;
			node["Name"] = v.Name;
			node["Read"] = v.Read;
			node["Write"] = v.Write;
			node["GPUBufferType"] = v.GPUBufferType;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialBlock &v) {
			v.Fields = node["Fields"].as<std::vector<::Imagine::MaterialField>>();
			v.Name = node["Name"].as<std::string>();
			v.Read = node["Read"].as<bool>();
			v.Write = node["Write"].as<bool>();
			v.GPUBufferType = node["GPUBufferType"].as<Imagine::MaterialBlock::BufferType>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::MaterialSet> {
		inline static Node encode(const ::Imagine::MaterialSet &v) {
			Node node;
			node["Blocks"] = v.Blocks;
			node["Stages"] = v.Stages;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialSet &v) {
			v.Blocks = node["Blocks"].as<std::vector<Imagine::MaterialBlock>>();
			v.Stages = node["Stages"].as<Imagine::ShaderStage>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::MaterialPushConstant> {
		inline static Node encode(const ::Imagine::MaterialPushConstant &v) {
			Node node;
			node["Block"] = v.Block;
			node["Stages"] = v.Stages;
			node["Offset"] = v.offset;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialPushConstant &v) {
			v.Block = node["Block"].as<Imagine::MaterialBlock>();
			v.Stages = node["Stages"].as<Imagine::ShaderStage>();
			v.offset = node["Offset"].as<uint32_t>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::MaterialLayout> {
		inline static Node encode(const ::Imagine::MaterialLayout &v) {
			Node node;
			node["Sets"] = v.Sets;
			node["PushConstants"] = v.PushConstants;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::MaterialLayout &v) {
			v.Sets = node["Sets"].as<std::vector<Imagine::MaterialSet>>();
			v.PushConstants = node["PushConstants"].as<std::vector<Imagine::MaterialPushConstant>>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::CPUMaterial> {
		inline static Node encode(const ::Imagine::CPUMaterial &v) {
			Node node;
			node["Handle"] = v.Handle;
			node["Layout"] = v.layout;
			node["Shaders"] = v.shaders;
			node["Pass"] = v.pass;
			node["Topology"] = v.topology;
			node["Mode"] = v.mode;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::CPUMaterial &v) {
			v.Handle = node["Handle"].as<Imagine::AssetHandle>();
			v.layout = node["Layout"].as<Imagine::MaterialLayout>();
			v.shaders = node["Shaders"].as<std::array<Imagine::AssetHandle, 5>>();
			v.pass = node["Pass"].as<Imagine::MaterialPass>();
			v.topology = node["Topology"].as<Imagine::Topology>();
			v.mode = node["Mode"].as<Imagine::MaterialFilling>();
			return true;
		}
	};

} // namespace YAML
