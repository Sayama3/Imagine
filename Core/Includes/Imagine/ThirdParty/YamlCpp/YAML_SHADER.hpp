//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include "YAML_DEFINE.hpp"

#include "Imagine/Rendering/CPU/CPUShader.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"


namespace YAML {
	YAML_SIMPLE_MGN_ENUM_EMITTER(ShaderStage);

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUFileShader &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Type", "File");
			YAML_OUT_VAL_STR("Path", path);
			YAML_OUT_VAL_STR("Stage", stage);
		}
		out << YAML::EndMap;
		return out;
	};

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUMemoryShader &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Type", "Memory");
			YAML_OUT_VAL_STR("Name", name);
			YAML_OUT_VAL_STR("Shader", shader);
			YAML_OUT_VAL_STR("Stage", stage);
		}
		out << YAML::EndMap;
		return out;
	};

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUShader &v) {
		if (auto *shader = dynamic_cast<const Imagine::CPUFileShader *>(&v)) {
			out << *shader;
		}
		else if (auto *shader = dynamic_cast<const Imagine::CPUMemoryShader *>(&v)) {
			out << *shader;
		}
		else {
			MGN_CORE_CASSERT(false);
			out << YAML::BeginMap;
			out << KEYVAL("Type", "Unknown");
			YAML_OUT_VAL_STR("Stage", stage);
			out << YAML::EndMap;
		}
		return out;
	};
} // namespace YAML

namespace YAML {
	YAML_SIMPLE_MGN_ENUM_CONVERTER(ShaderStage);

	template<>
	struct convert<Imagine::CPUFileShader> {
		inline static Node encode(const Imagine::CPUFileShader &v) {
			Node node;
			node["Type"] = "FileShader";
			YAML_NODE_OUT_VAL_STR("Path", path);
			YAML_NODE_OUT_VAL_STR("Stage", stage);
			return node;
		}

		inline static bool decode(const Node &node, Imagine::CPUFileShader &v) {
			MGN_CORE_CASSERT(node["Type"].as<std::string>() == "FileShader");
			YAML_NODE_IN_VAL_STR("Stage", stage, ShaderStage);
			YAML_NODE_IN_VAL_STR("Path", path, Path);
			return true;
		}
	};

	template<>
	struct convert<Imagine::CPUMemoryShader> {
		inline static Node encode(const Imagine::CPUMemoryShader &v) {
			Node node;
			node["Type"] = "MemoryShader";
			YAML_NODE_OUT_VAL_STR("Stage", stage);
			YAML_NODE_OUT_VAL_STR("Name", name);
			YAML_NODE_OUT_VAL_STR("Shader", shader);
			return node;
		}

		inline static bool decode(const Node &node, Imagine::CPUMemoryShader &v) {
			MGN_CORE_CASSERT(node["Type"].as<std::string>() == "MemoryShader");
			YAML_NODE_IN_VAL_STR("Stage", stage, ShaderStage);
			YAML_NODE_IN_GLB_VAL_STR("Name", name, std::string);
			YAML_NODE_IN_VAL_STR("Shader", shader, Buffer);
			return true;
		}
	};

	template<>
	struct convert<Imagine::CPUShader> {
		inline static Node encode(const Imagine::CPUShader &v) {
			Node node;
			if (auto *shader = dynamic_cast<const Imagine::CPUFileShader *>(&v)) {
				return convert<Imagine::CPUShader>::encode(*shader);
			}
			else if (auto *shader = dynamic_cast<const Imagine::CPUMemoryShader *>(&v)) {
				return convert<Imagine::CPUMemoryShader>::encode(*shader);
			}
			else {
				MGN_CORE_CASSERT(false);
				node["Type"] = "Unknown";
				YAML_NODE_OUT_VAL_STR("Stage", stage);
			}
			return node;
		}

		inline static bool decode(const Node &node, Imagine::CPUShader &v) {
			if (auto *shader = dynamic_cast<Imagine::CPUFileShader *>(&v)) {
				return convert<Imagine::CPUShader>::decode(node, *shader);
			}
			else if (auto *shader = dynamic_cast<Imagine::CPUMemoryShader *>(&v)) {
				return convert<Imagine::CPUMemoryShader>::decode(node, *shader);
			}
			else {
				MGN_CORE_CASSERT(false);
				YAML_NODE_IN_VAL_STR("Stage", stage, ShaderStage)
			}
			return true;
		}
	};

} // namespace YAML
