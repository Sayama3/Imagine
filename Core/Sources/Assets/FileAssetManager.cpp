//
// Created by Iannis on 05/07/2025.
//

#include "Imagine/Assets/FileAssetManager.hpp"

#include "Imagine/Assets/AssetImporter.hpp"
#include "Imagine/Rendering/CPU/CPUModel.hpp"
#include "Imagine/Scene/Scene.hpp"
#include "Imagine/ThirdParty/YamlCpp.hpp"

namespace Imagine {
	bool FileAssetManager::IsAssetHandleValid(AssetHandle handle) const {
		MGN_PROFILE_FUNCTION();
		return handle && (m_AssetRegistry.contains(handle) || m_MemoryAssets.contains(handle));
	}

	bool FileAssetManager::IsAssetLoaded(AssetHandle handle) const {
		MGN_PROFILE_FUNCTION();
		return m_LoadedAssets.contains(handle) || m_MemoryAssets.contains(handle);
	}

	bool FileAssetManager::IsAssetImported(const Path &path, AssetHandle *handle /* = nullptr*/) const {
		auto it = std::find_if(m_AssetRegistry.begin(), m_AssetRegistry.end(), [&path](std::pair<AssetHandle, AssetMetadata> assets) {
			return assets.second.FilePath.equivalent(path);
		});

		if (handle && it != m_AssetRegistry.end()) {
			*handle = it->first;
		}

		return it != m_AssetRegistry.end();
	}

	Ref<Asset> FileAssetManager::GetAsset(AssetHandle handle) {
		MGN_PROFILE_FUNCTION();
		if (!IsAssetHandleValid(handle)) {
			return nullptr;
		}

		Ref<Asset> asset = nullptr;

		if (IsAssetLoaded(handle)) {
			asset = GetLoadedAsset(handle);
		}
		else {
			const auto &metadata = GetMetadata(handle);
			asset = AssetImporter::ImportAsset(metadata);
			if (asset) {
				m_LoadedAssets.insert({handle, asset});
			}
			else {
				MGN_CORE_ERROR("Could not load the asset {0}", handle.string());
			}
		}

		return asset;
	}

	bool FileAssetManager::LoadAsset(AssetHandle handle) {
		if (!IsAssetLoaded(handle)) {
			const auto &metadata = GetMetadata(handle);
			if (auto asset = AssetImporter::ImportAsset(metadata)) {
				m_LoadedAssets.emplace(handle, std::move(asset));
				return true;
			}
			return false;
		}
		return true;
	}

	AssetType FileAssetManager::GetAssetType(AssetHandle handle) const {
		MGN_PROFILE_FUNCTION();
		if (!IsAssetHandleValid(handle)) {
			return AssetType::None;
		}

		if (IsAssetLoaded(handle)) {
			return GetLoadedAsset(handle)->GetType();
		}
		else {
			const auto &metadata = GetMetadata(handle);
			return metadata.Type;
		}
	}

	Ref<Asset> FileAssetManager::ImportAsset(const Path &assetPath, AssetType hint /* = AssetType::None*/) {
		if (!FileSystem::Exist(assetPath) || AssetImporter::GetAssetType(assetPath) == AssetType::None) {
			//				MGN_CORE_WARN("The path '{0}' either doesn't exist or cannot be converted.", assetPath.string());
			return nullptr;
		}

		AssetMetadata metadata;

		metadata.FilePath = assetPath;
		metadata.Type = AssetImporter::HasAssetType(assetPath, hint) ? hint : AssetImporter::GetAssetType(assetPath);
		auto asset = AssetImporter::ImportAsset(metadata);

		if (asset) {
			m_LoadedAssets[metadata.Handle] = asset;
			m_AssetRegistry[metadata.Handle] = metadata;
			// TODO: Save asset manager.
		}

		return asset;
	}

	Ref<Asset> FileAssetManager::GetOrCreateAsset(const Path &assetPath, AssetType hint /* = AssetType::None*/) {
		MGN_PROFILE_FUNCTION();

		Ref<Asset> asset = nullptr;

		const auto it = std::find_if(m_AssetRegistry.begin(), m_AssetRegistry.end(), [&assetPath](const std::pair<AssetHandle, AssetMetadata> &assets) {
			return assets.second.FilePath.equivalent(assetPath);
		});

		if (it == m_AssetRegistry.end()) {
			asset = ImportAsset(assetPath);
		}
		else {
			asset = GetAsset(it->first);
			if (hint != AssetType::None && hint != asset->GetType()) {
				MGN_CORE_WARN("Asset '{}'({}) has type '{}', but the expected type is '{}'.", assetPath.string(), it->first.string(), AssetTypeToString(asset->GetType()), AssetTypeToString(hint));
			}
		}

		return asset;
	}

	Path FileAssetManager::GetFilePath(const AssetHandle handle) const {
		return GetMetadata(handle).FilePath;
	}

	const AssetMetadata &FileAssetManager::GetMetadata(const AssetHandle handle) const {
		MGN_PROFILE_FUNCTION();
		static AssetMetadata s_NullMetadata;
		auto it = m_AssetRegistry.find(handle);
		if (it == m_AssetRegistry.end()) {
			return s_NullMetadata;
		}
		return it->second;
	}

	std::optional<AssetMetadata> FileAssetManager::GetMetadata(const Path &assetPath) const {
		MGN_PROFILE_FUNCTION();
		auto it = std::find_if(m_AssetRegistry.begin(), m_AssetRegistry.end(), [&assetPath](std::pair<AssetHandle, AssetMetadata> assets) {
			return assets.second.FilePath == assetPath;
		});

		return it != m_AssetRegistry.end() ? it->second : std::optional<AssetMetadata>{std::nullopt};
	}

	void FileAssetManager::SetPath(AssetHandle handle, Path newPath) {
		MGN_PROFILE_FUNCTION();

		auto it = m_AssetRegistry.find(handle);
		if (it != m_AssetRegistry.end()) {
			it->second.FilePath = newPath;
			// TODO: Save asset manager.
		}
	}

	bool FileAssetManager::AddAsset(Ref<Asset> asset) {
		MGN_PROFILE_FUNCTION();
		if (!asset) return false;
		m_MemoryAssets.emplace(asset->Handle, asset);
		return true;
	}

	bool FileAssetManager::AddAsset(Ref<Asset> asset, const Path &path) {
		MGN_PROFILE_FUNCTION();
		if (!asset) return false;
		AssetMetadata metadata;
		metadata.Handle = asset->Handle;
		metadata.FilePath = path;
		metadata.Type = asset->GetType();
		m_AssetRegistry.emplace(metadata.Handle, metadata);
		m_LoadedAssets.emplace(metadata.Handle, asset);
		// TODO: Save asset manager.
		return true;
	}

	bool FileAssetManager::SaveMemoryAsset(AssetHandle handle, const Path &path) {
		MGN_PROFILE_FUNCTION();
		auto it = m_MemoryAssets.find(handle);
		if (it == m_MemoryAssets.end()) return false;

		Ref<Asset> asset = it->second;
		if (!asset) return false;

		AssetMetadata metadata;
		metadata.Handle = handle;
		metadata.FilePath = path;
		metadata.Type = asset->GetType();

		m_MemoryAssets.erase(it);
		m_AssetRegistry.emplace(metadata.Handle, metadata);
		m_LoadedAssets.emplace(metadata.Handle, asset);
		// TODO: Save asset manager.
		return true;
	}

	bool FileAssetManager::RemoveAsset(AssetHandle handle) {
		MGN_PROFILE_FUNCTION();
		if (!IsAssetHandleValid(handle)) return false;

		auto loaded_it = m_LoadedAssets.find(handle);
		if (loaded_it != m_LoadedAssets.end()) {
			m_LoadedAssets.erase(loaded_it);
		}

		auto memory_it = m_MemoryAssets.find(handle);
		if (memory_it != m_MemoryAssets.end()) {
			m_MemoryAssets.erase(memory_it);
		}

		auto registry_it = m_AssetRegistry.find(handle);
		if (registry_it != m_AssetRegistry.end()) {
			m_AssetRegistry.erase(registry_it);
		}

		// TODO: Save asset manager.
		return true;
	}

	void FileAssetManager::UnloadAsset(AssetHandle handle) {
		MGN_PROFILE_FUNCTION();
		if (!IsAssetHandleValid(handle)) return;

		auto loaded_it = m_LoadedAssets.find(handle);
		if (loaded_it != m_LoadedAssets.end()) {
			if(loaded_it->second->GetType() == AssetType::Model) {
				if(Ref<CPUModel> model = CastPtr<CPUModel>(loaded_it->second)) {
					for(auto& m : model->Meshes) {
						UnloadAsset(m->Handle);
					}
					for(auto& m : model->Textures) {
						UnloadAsset(m->Handle);
					}
					for(auto& m : model->Instances) {
						UnloadAsset(m->Handle);
					}
				}
			}
			m_LoadedAssets.erase(loaded_it);
		}

		auto memory_it = m_MemoryAssets.find(handle);
		if (memory_it != m_MemoryAssets.end()) {
			if(memory_it->second->GetType() == AssetType::Model) {
				if(Ref<CPUModel> model = CastPtr<CPUModel>(memory_it->second)) {
					for(auto& m : model->Meshes) {
						UnloadAsset(m->Handle);
					}
					for(auto& m : model->Textures) {
						UnloadAsset(m->Handle);
					}
					for(auto& m : model->Instances) {
						UnloadAsset(m->Handle);
					}
				}
			}
			m_MemoryAssets.erase(memory_it);
		}
	}

	Ref<Asset> FileAssetManager::GetLoadedAsset(const AssetHandle handle) const {
		MGN_PROFILE_FUNCTION();
		if (m_LoadedAssets.contains(handle)) {
			return m_LoadedAssets.at(handle);
		}
		else if (m_MemoryAssets.contains(handle)) {
			return m_MemoryAssets.at(handle);
		}
		return nullptr;
	}

} // namespace Imagine

namespace Imagine {

	bool FileAssetManagerSerializer::SerializeReadable(const FileAssetManager *manager, const std::filesystem::path &filePath) {
		MGN_PROFILE_FUNCTION();
		YAML::Emitter e;
		e << YAML::BeginMap;
		{
			e << KEYVAL("Type", "File Asset Manager");
			e << KEYVAL("Assets", YAML::BeginSeq);
			for (const auto &[id, metadata]: manager->m_AssetRegistry) {
				e << metadata;
			}
			e << YAML::EndSeq;
		}
		e << YAML::EndMap;

		std::filesystem::create_directories(filePath.parent_path());
		ThirdParty::YamlCpp::WriteYamlFile(filePath, e);
		return true;
	}

	Scope<FileAssetManager> FileAssetManagerSerializer::DeserializeReadable(const std::filesystem::path &filePath) {
		MGN_PROFILE_FUNCTION();
		if (!std::filesystem::exists(filePath) || !std::filesystem::is_regular_file(filePath)) return nullptr;

		const YAML::Node node = ThirdParty::YamlCpp::ReadFileAsYAML(filePath);
		if (!node["Type"] || node["Type"].as<std::string>() != "File Asset Manager") return nullptr;

		Scope<FileAssetManager> manager = CreateScope<FileAssetManager>();

		if (auto assetsNodes = node["Assets"]) {
			for (auto metadataNode: assetsNodes) {
				auto metadata = metadataNode.as<AssetMetadata>();
				manager->m_AssetRegistry[metadata.Handle] = metadata;
			}
			return std::move(manager);
		}
		return nullptr;
	}
} // namespace Imagine
