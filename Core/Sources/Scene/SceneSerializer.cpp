//
// Created by ianpo on 11/07/2025.
//

#include "Imagine/Scene/SceneSerializer.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/ThirdParty/YamlCpp.hpp"
#include "Imagine/ThirdParty/YamlCpp/YAML_SCENE.hpp"

namespace Imagine {

	void SceneSerializer::SerializeReadable(Scene *scene, const std::filesystem::path &folderPath) {
		std::filesystem::create_directories(folderPath);

		const std::filesystem::path entitiesPath = folderPath / c_EntitiesFolder;
		const std::filesystem::path componentsPath = folderPath / c_ComponentsFolder;

		FileSystem::WriteTextFile(folderPath / "scene.mgn", scene->Handle.string());

		std::filesystem::create_directory(entitiesPath);
		std::filesystem::create_directory(componentsPath);

		// Components Serialization
		{
			for (auto &[ccId, cc]: scene->m_CustomComponents) {
				const Scene::Metadata &metadata = scene->m_CustomComponentsMetadata.at(ccId);
				const std::filesystem::path ccPath = componentsPath / (metadata.id.raw_string() + ".mgn");
				YAML::Emitter out;
				out << YAML::BeginMap;
				{
					out << KEYVAL("Type", "Component");
					out << KEYVAL("Metadata", metadata);
					out << KEYVAL("Count", cc.Count());
				}
				out << YAML::EndMap;
				ThirdParty::YamlCpp::WriteYamlFile(ccPath, out);
			}
		}

		// Entity Serialization

		{
			for (uint32_t i = 0; i < scene->m_SparseEntities.Count(); ++i) {
				const auto id = scene->m_SparseEntities.GetID(i);
				Entity &e = scene->m_SparseEntities.Get(id);
				const std::filesystem::path ePath = entitiesPath / (e.Id.string() + ".mgn");
				YAML::Emitter out;
				out << YAML::BeginMap;
				{
					out << KEYVAL("Type", "Entity");
					out << KEYVAL("ID", e.Id);
					out << KEYVAL("Name", scene->m_Names.Get(id).c_str());
					out << KEYVAL("Local Position", e.LocalPosition);
					out << KEYVAL("Local Rotation", e.LocalRotation);
					out << KEYVAL("Local Scale", e.LocalScale);
					out << KEYVAL("Custom Components", YAML::BeginMap);
					{
						for (auto &[ccId, cc]: scene->m_CustomComponents) {
							if (!cc.Exist(id)) continue;
							const Scene::Metadata &metadata = scene->m_CustomComponentsMetadata.at(ccId);
							out << KEYVAL(metadata.id, cc.GetConstView(id));
						}
					}
					out << YAML::EndMap;

					out << KEYVAL("Relationship", YAML::BeginMap);
					{
						out << KEYVAL("Root", scene->m_Roots.contains(id));
						if (scene->m_Parents.Exist(id)) out << KEYVAL("Parent", scene->m_Parents.Get(id).parent);
						if (scene->m_Children.Exist(id)) out << KEYVAL("Child", scene->m_Children.Get(id).firstChild);
						if (scene->m_Siblings.Exist(id)) {
							out << KEYVAL("Next", scene->m_Siblings.Get(id).next);
							out << KEYVAL("Previous", scene->m_Siblings.Get(id).previous);
						}
					}
					out << YAML::EndMap;
				}
				out << YAML::EndMap;
				ThirdParty::YamlCpp::WriteYamlFile(ePath, out);
			}
		}
	}

	Scene *SceneSerializer::DeserializeReadable(const std::filesystem::path &folderPath) {
		if (!std::filesystem::exists(folderPath / "scene.mgn")) return nullptr;
		std::string content = FileSystem::ReadTextFile(folderPath / "scene.mgn");
		std::regex self_regex("(\\d+)_(\\d+)", std::regex_constants::ECMAScript | std::regex_constants::icase);
		union {
			AssetHandle id{NULL_ASSET_HANDLE};
			std::array<uint64_t, 2> ids;
		};

		uint32_t count{0};
		for (auto it = std::sregex_iterator(content.begin(), content.end(), self_regex); it != std::sregex_iterator(); ++it) {
			ids[count++] = std::stoull(it->str());
		}
		if (id == NULL_ASSET_HANDLE) return nullptr;

		Scene *scene = new Scene();
		scene->Handle = id;

		const std::filesystem::path entitiesPath = folderPath / c_EntitiesFolder;
		const std::filesystem::path componentsPath = folderPath / c_ComponentsFolder;

		for (std::filesystem::directory_iterator it(componentsPath, std::filesystem::directory_options::skip_permission_denied); it != std::filesystem::directory_iterator(); ++it) {
			if (it->path().extension() != ".mgn") continue;
			const YAML::Node node = ThirdParty::YamlCpp::ReadFileAsYAML(it->path());
			if (node["Type"].as<std::string>() != "Component") continue;
			Scene::Metadata metadata = node["Metadata"].as<Scene::Metadata>();

			const uint32_t count = node["Count"].as<uint32_t>();
			if (!scene->m_CustomComponents.contains(metadata.id)) {
				scene->m_CustomComponentsMetadata[metadata.id] = metadata;
				scene->m_CustomComponents[metadata.id] = RawSparseSet<>{(uint32_t) metadata.size, count};
			}
			else {
				scene->m_CustomComponents.at(metadata.id).Reserve(count);
			}
		}

		for (std::filesystem::directory_iterator it(entitiesPath, std::filesystem::directory_options::skip_permission_denied); it != std::filesystem::directory_iterator(); ++it) {
			if (it->path().extension() != ".mgn") continue;
			const YAML::Node node = ThirdParty::YamlCpp::ReadFileAsYAML(it->path());
			if (node["Type"].as<std::string>() != "Entity") continue;
			const EntityID eId = node["ID"].as<EntityID>();
			Entity e;
			e.Id = eId;
			e.LocalPosition = node["Local Position"].as<Vec3>();
			e.LocalRotation = node["Local Rotation"].as<Quat>();
			e.LocalScale = node["Local Scale"].as<Vec3>();
			const std::string name = node["Name"].as<std::string>();
			scene->m_SparseEntities.Create(eId.id, e);
			scene->m_Names.Create(eId.id, name);

			if (auto relationshipNode = node["Relationship"]) {
				const bool isRoot = relationshipNode["Root"].as<bool>();
				if (isRoot) scene->m_Roots.emplace(eId);

				if (auto parentNode = relationshipNode["Parent"]) {
					const EntityID parentId = parentNode.as<EntityID>(EntityID::NullID);
					if (parentId.IsValid()) {
						scene->m_Parents.Create(eId.id, Parent{parentId});
					}
				}

				if (auto childNode = relationshipNode["Child"]) {
					const EntityID childId = childNode.as<EntityID>(EntityID::NullID);
					if (childId.IsValid()) {
						scene->m_Children.Create(eId.id, Child{childId});
					}
				}

				if (auto nextNode = relationshipNode["Next"]) {
					const EntityID nextId = nextNode.as<EntityID>(EntityID::NullID);
					if (nextId.IsValid()) {
						scene->m_Siblings.GetOrCreate(eId.id, {EntityID::NullID, EntityID::NullID}).next = nextId;
					}
				}

				if (auto previousNode = relationshipNode["Previous"]) {
					const EntityID previousId = previousNode.as<EntityID>(EntityID::NullID);
					if (previousId.IsValid()) {
						scene->m_Siblings.GetOrCreate(eId.id, {EntityID::NullID, EntityID::NullID}).previous = previousId;
					}
				}
			}

			if (auto ccNode = node["Custom Components"]) {
				for (const YAML::detail::iterator_value &iterator_Value: ccNode) {
					UUID ccId = iterator_Value.first.as<UUID>();
					Buffer buffer = iterator_Value.second.as<Buffer>();
					if (!scene->m_CustomComponents.contains(ccId)) continue;
					auto &cc = scene->m_CustomComponents.at(ccId);
					cc.Create(eId.id, buffer);
				}
			}
		}

		return scene;
	}
} // namespace Imagine
