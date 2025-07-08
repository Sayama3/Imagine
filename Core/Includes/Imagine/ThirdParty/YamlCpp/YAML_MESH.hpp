//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include "YAML_ASSET.hpp"
#include "YAML_DEFINE.hpp"
#include "YAML_MATH.hpp"

#include "Imagine/Rendering/MeshParameters.hpp"

namespace YAML {
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::Topology &v) {
		out << Imagine::Core::TopologyToString(v);
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::Vertex &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Position", v.position);
			out << KEYVAL("Normal", v.normal);
			out << KEYVAL("Tangent", glm::fvec3(v.tangent));
			out << KEYVAL("Bitangent", glm::fvec3(v.bitangent));
			out << KEYVAL("UV", glm::fvec2(v.uv_x, v.uv_y));
			out << KEYVAL("Color", v.color);
		}
		out << YAML::EndMap;
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::LOD &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Index", v.index);
			out << KEYVAL("Count", v.count);
			out << KEYVAL("Material Instance", v.materialInstance);
		}
		out << YAML::EndMap;
		return out;
	}
} // namespace Imagine::ThirdParty::YamlCpp

namespace YAML {
	template<>
	struct convert<::Imagine::Core::Topology> {
		inline static Node encode(const ::Imagine::Core::Topology &v) {
			Node node;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::Topology &v) {
			return ::Imagine::Core::TryTopologyFromString(node.as<std::string>(), v);
		}
	};
	template<>
	struct convert<::Imagine::Core::Vertex> {
		inline static Node encode(const ::Imagine::Core::Vertex &v) {
			Node node;
			node["Position"] = v.position;
			node["Normal"] = v.normal;
			node["Tangent"] = glm::fvec3(v.tangent);
			node["Bitangent"] = glm::fvec3(v.bitangent);
			node["UV"] = glm::fvec2(v.uv_x, v.uv_y);
			node["Color"] = v.color;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::Vertex &v) {
			v.position = node["Position"].as<glm::fvec3>();
			v.normal = node["Normal"].as<glm::fvec3>();
			v.tangent = glm::fvec4(node["Tangent"].as<glm::fvec3>(), 0);
			v.bitangent = glm::fvec4(node["Bitangent"].as<glm::fvec3>(), 0);
			v.color = node["Color"].as<glm::fvec4>();
			const auto uv = node["UV"].as<glm::fvec2>();
			v.uv_x = uv.x;
			v.uv_y = uv.y;
			return true;
		}
	};
	template<>
	struct convert<::Imagine::Core::LOD> {
		inline static Node encode(const ::Imagine::Core::LOD &v) {
			Node node;
			node["Index"] = v.index;
			node["Count"] = v.count;
			node["Material Instance"] = v.materialInstance;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::Core::LOD &v) {
			v.index = node["Index"].as<uint32_t>();
			v.count = node["Count"].as<uint32_t>();
			v.materialInstance = node["Material Instance"].as<Imagine::Core::AssetHandle>();
			return true;
		}
	};
} // namespace YAML
