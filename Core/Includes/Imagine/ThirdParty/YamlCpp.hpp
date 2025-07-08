//
// Created by ianpo on 07/07/2025.
//

#pragma once


#include "YamlCpp/YAML_DEFINE.hpp"
#include "YamlCpp/YAML_STD.hpp"
#include "YamlCpp/YAML_CORE.hpp"
#include "YamlCpp/YAML_MATH.hpp"
#include "YamlCpp/YAML_ASSET.hpp"
#include "YamlCpp/YAML_VIRTUAL_TEXTURE.hpp"
#include "YamlCpp/YAML_MESH.hpp"
#include "YamlCpp/YAML_SHADER.hpp"
#include "YamlCpp/YAML_MATERIAL.hpp"
#include "YamlCpp/YAML_MATERIAL_INSTANCE.hpp"


namespace Imagine::ThirdParty {
	namespace YamlCpp {
		void WriteYamlFile(const std::filesystem::path& path, YAML::Emitter& emitter);
		void WriteYamlFile(const Core::Path& path, YAML::Emitter& emitter);
		YAML::Node ReadFileAsYAML(const std::filesystem::path& path);
		YAML::Node ReadFileAsYAML(const Core::Path& path);
	}
} // namespace Imagine::ThirdParty

