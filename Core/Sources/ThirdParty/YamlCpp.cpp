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

		YAML::Node ReadFileAsYAML(const std::filesystem::path &path) {
			MGN_PROFILE_FUNCTION();
			YAML::Node data;
			std::ifstream ifstream(path);
			std::stringstream  stringstream;
			stringstream << ifstream.rdbuf();
			data = YAML::Load(stringstream.str());
			return data;
		}

		YAML::Emitter &operator<<(YAML::Emitter &out, const Core::AssetHandle &v) {
			out << v.GetID();
			return out;
		}

		YAML::Emitter &operator<<(YAML::Emitter &out, const Core::AssetType &rhs) {
			out << Core::AssetTypeToString(rhs);
			return out;
		}
	} // namespace YamlCpp
}
