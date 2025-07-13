//
// Created by Iannis on 05/07/2025.
//

#pragma once

#include "AssetHandle.hpp"
#include "AssetManagerBase.hpp"
#include "AssetMetadata.hpp"
#include "Imagine/Core/InternalCore.hpp"

namespace Imagine {
#ifdef MGN_DIST
	using AssetRegistry = std::unordered_map<AssetHandle, AssetMetadata>;
	using AssetRegistryIterator = std::unordered_map<AssetHandle, AssetMetadata>::iterator;
#else
	// ordered map is cool for debugging
	using AssetRegistry = std::map<AssetHandle, AssetMetadata>;
	using AssetRegistryIterator = std::map<AssetHandle, AssetMetadata>::iterator;
#endif
	class ProjectLayer;
	class FileAssetManagerSerializer;

	class FileAssetManager final : public AssetManagerBase {
		friend ProjectLayer;
		friend FileAssetManagerSerializer;
	public:
		[[nodiscard]] virtual bool IsAssetHandleValid(AssetHandle handle) const override;
		[[nodiscard]] virtual bool IsAssetLoaded(AssetHandle handle) const override;
		[[nodiscard]] virtual AssetType GetAssetType(AssetHandle handle) const override;
		[[nodiscard]] virtual Ref<Asset> GetAsset(AssetHandle handle) override;
		virtual bool LoadAsset(AssetHandle handle) override;
		virtual void UnloadAsset(AssetHandle handle) override;
		virtual bool AddAsset(Ref<Asset> asset) override;
		virtual bool RemoveAsset(AssetHandle handle) override;

		[[nodiscard]] virtual const AssetMetadata &GetMetadata(AssetHandle handle) const;

	public:
		[[nodiscard]] std::optional<AssetMetadata> GetMetadata(const Path& assetPath) const;
		[[nodiscard]] bool IsAssetImported(const Path &path, AssetHandle *handle = nullptr) const;

		Ref<Asset> ImportAsset(const Path &assetPath, AssetType hint = AssetType::None);
		Ref<Asset> GetOrCreateAsset(const Path &assetPath, AssetType hint = AssetType::None);

		void SetPath(AssetHandle handle, Path newPath);
		[[nodiscard]] Path GetFilePath(AssetHandle) const;

		bool AddAsset(Ref<Asset> asset, const Path &path);
		bool SaveMemoryAsset(AssetHandle handle, const Path &path);

	public:
		Ref<Asset> GetLoadedAsset(AssetHandle handle) const;

		template<typename T, typename... Args>
		Ref<T> CreateMemoryAsset(Args &&...args);

		template<typename T, typename... Args>
		Ref<T> CreateAsset(Path path, Args &&...args);
	private:
		AssetMap m_LoadedAssets;
		AssetMap m_MemoryAssets;
		AssetRegistry m_AssetRegistry;
	};


	template<typename T, typename... Args>
	Ref<T> FileAssetManager::CreateMemoryAsset(Args &&...args)
	{
		Ref<T> asset = CreateRef<T>(std::forward<Args>(args)...);
		m_MemoryAssets.emplace(asset->Handle, asset);
		return asset;
	}

	template<typename T, typename ...Args>
	inline Ref<T> FileAssetManager::CreateAsset(Path path, Args&&... args)
	{
		AssetMetadata metadata;
		metadata.FilePath = std::move(path);
		metadata.Type = T::GetStaticType();
		Ref<T> asset = CreateRef<T>(std::forward<Args>(args)...);
		asset->Handle = metadata.Handle;
		m_AssetRegistry.emplace(metadata.Handle, metadata);
		m_LoadedAssets.emplace(metadata.Handle, asset);
		// TODO: Save asset manager.
		return asset;
	}

	class FileAssetManagerSerializer {
	public:
		static bool SerializeReadable(const FileAssetManager* manager, const std::filesystem::path& filePath);
		static Scope<FileAssetManager> DeserializeReadable(const std::filesystem::path& filePath);
	};

} // namespace Imagine
