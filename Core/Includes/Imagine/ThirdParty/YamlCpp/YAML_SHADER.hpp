//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include "YAML_DEFINE.hpp"

#include "Imagine/Rendering/ShaderParameters.hpp"


namespace YAML {
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::ShaderStage &v) {
		out << Imagine::Core::ShaderStageToString(v);
		return out;
	}
} // namespace Imagine::ThirdParty::YamlCpp

namespace YAML {
	template<>
	struct convert<Imagine::Core::ShaderStage> {
		inline static Node encode(const Imagine::Core::ShaderStage &rhs) {
			Node node;
			node = Imagine::Core::ShaderStageToString(rhs);
			return node;
		}
		inline static bool decode(const Node &node, Imagine::Core::ShaderStage &rhs) {
			rhs = Imagine::Core::ShaderStageFromString(node.as<std::string>());
			return true;
		}
	};
}