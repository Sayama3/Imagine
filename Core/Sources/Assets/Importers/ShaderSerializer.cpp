//
// Created by ianpo on 12/04/2024.
//

#include "Imagine/Assets/Importers/ShaderSerializer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/ThirdParty/YamlCpp.hpp"


namespace Imagine::Core
{
	static inline constexpr std::array<const char* const, 17> Extensions {".glsl", ".shader", ".compute", ".comp", ".cs", ".vert", ".vs", ".tessco", ".tcs", ".tessev", ".tes", ".geom", ".gs", ".frag", ".fs", ".pixel", ".ps"};

	bool ShaderSerializer::IsShaderSource(const std::filesystem::path &path) {
		return path.filename().string().ends_with(ShaderExtension);
	}

	Ref<Asset> ShaderSerializer::ImportReadableShaderSource(const AssetMetadata &metadata) {
		MGN_CORE_CASSERT(IsShaderSource(metadata.FilePath.GetFullPath()));
		const YAML::Node node = ThirdParty::YamlCpp::ReadFileAsYAML(metadata.FilePath);
		if (node["Type"].as<std::string>() == "FileShader") {
			auto shader = CreateRef<CPUFileShader>();
			shader->Handle = metadata.Handle;
			*shader = node["Data"].as<CPUFileShader>();
			return shader;
		}
		else if (node["Type"].as<std::string>() == "MemoryShader") {
			auto shader = CreateRef<CPUMemoryShader>();
			shader->Handle = metadata.Handle;
			*shader = node["Data"].as<CPUMemoryShader>();
			return shader;
		}
		return nullptr;
	}

	void ShaderSerializer::ExportReadableShaderSource(const AssetMetadata &metadata, Ref<CPUShader> source) {
		if (!source) return;
		YAML::Emitter out;
		out << YAML::BeginMap;
		if (auto *shader = dynamic_cast<const Imagine::Core::CPUFileShader *>(source.get())) {
			out << KEYVAL("Type", "FileShader");
			out << KEYVAL("Data", *shader);
		} else if (auto *shader = dynamic_cast<const Imagine::Core::CPUMemoryShader *>(source.get())) {
			out << KEYVAL("Type", "MemoryShader");
			out << KEYVAL("Data", *shader);
		}
		out << YAML::EndMap;
		ThirdParty::YamlCpp::WriteYamlFile(metadata.FilePath, out);
	}

} // namespace Imagine::Core
