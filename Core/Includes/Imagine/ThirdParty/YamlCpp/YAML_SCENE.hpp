//
// Created by ianpo on 13/07/2025.
//

#pragma once

#include "YAML_DEFINE.hpp"
#include "YAML_MATH.hpp"
#include "YAML_STD.hpp"
#include "YAML_CORE.hpp"

#include "Imagine/Rendering/MeshParameters.hpp"
#include "Imagine/Scene/Entity.hpp"
#include "Imagine/Scene/Scene.hpp"

namespace YAML {
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::EntityID &v) {
		out << v.id;
		return out;
	};
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Scene::Metadata &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Name", v.name);
			out << KEYVAL("Size", v.size);
			out << KEYVAL("UUID", v.id);
		}
		out << YAML::EndMap;
		return out;
	};
}

namespace YAML {
	template<>
	struct convert<Imagine::EntityID> {
		inline static Node encode(const Imagine::EntityID &v) {
			Node node;
			node = v.id;
			return node;
		}
		inline static bool decode(const Node &node, Imagine::EntityID &v) {
			v.id = node.as<uint32_t>();
			return true;
		}
	};

	template<>
	struct convert<Imagine::Scene::Metadata> {
		inline static Node encode(const Imagine::Scene::Metadata &v) {
			Node node;
			node["Name"] = v.name;
			node["Size"] = v.size;
			node["UUID"] = v.id;
			return node;
		}

		inline static bool decode(const Node &node, Imagine::Scene::Metadata &v) {
			v.name = node["Name"].as<std::string>();
			v.size = node["Size"].as<uint64_t>();
			v.id = node["UUID"].as<Imagine::UUID>();
			return true;
		}
	};
}
