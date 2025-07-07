//
// Created by ianpo on 07/07/2025.
//

#pragma once


#include "YAML_DEFINE.hpp"
#include "YAML_STD.hpp"
#include "YAML_CORE.hpp"
#include "YAML_MATH.hpp"
#include "YAML_ASSET.hpp"
#include "YAML_MESH.hpp"
#include "YAML_SHADER.hpp"
#include "YAML_MATERIAL.hpp"
#include "YAML_MATERIAL_INSTANCE.hpp"


namespace Imagine::ThirdParty {
	namespace YamlCpp {
		void WriteYamlFile(const std::filesystem::path& path, YAML::Emitter& emitter);
		YAML::Node ReadFileAsYAML(const std::filesystem::path& path);
	}
} // namespace Imagine::ThirdParty

