//
// Created by ianpo on 07/07/2025.
//

#include "Imagine/ThirdParty/YamlCpp.hpp"

namespace Imagine::ThirdParty {
	namespace YamlCpp {
		void WriteYamlFile(const std::filesystem::path &path, YAML::Emitter &emitter) {
			MGN_PROFILE_FUNCTION();
			std::ofstream fileOut(path);
			fileOut << emitter.c_str();
		}
		void WriteYamlFile(const Path &path, YAML::Emitter &emitter) {
			WriteYamlFile(path.GetFullPath(), emitter);
		}

		YAML::Node ReadFileAsYAML(const std::filesystem::path &path) {
			MGN_PROFILE_FUNCTION();
			YAML::Node data;
			std::ifstream ifstream(path);
			std::stringstream stringstream;
			stringstream << ifstream.rdbuf();
			data = YAML::Load(stringstream.str());
			return data;
		}
		YAML::Node ReadFileAsYAML(const Path &path) {
			return ReadFileAsYAML(path.GetFullPath());
		}

	} // namespace YamlCpp
}
