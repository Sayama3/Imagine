//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include <utility>

#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Assets/AssetType.hpp"
#include "Imagine/Assets/AssetHandle.hpp"


namespace Imagine::Core
{
	struct AssetMetadata {
		inline AssetMetadata() = default;
		inline ~AssetMetadata() = default;
		inline AssetMetadata(Path p, const AssetType t) : FilePath(std::move(p)), Handle(), Type(t){}
		inline AssetMetadata(const AssetMetadata&) = default;
		inline AssetMetadata& operator=(const AssetMetadata&) = default;
		inline AssetMetadata(AssetMetadata&& o) noexcept = default;
		inline AssetMetadata& operator=(AssetMetadata&& o) noexcept = default;
		void swap(AssetMetadata& o) noexcept {
			std::swap(FilePath, o.FilePath);
			std::swap(Handle, o.Handle);
			std::swap(Type, o.Type);
		}

		inline operator bool() const {
			return Type != AssetType::None && Handle != NULL_ASSET_HANDLE;
		}
		inline bool operator ==(const AssetMetadata& other ) const {
			return Handle == other.Handle && Type == other.Type && FilePath == other.FilePath;
		}
		inline bool operator !=(const AssetMetadata& other) const {
			return !(*this == other);
		}

		Path FilePath;
		AssetHandle Handle;
		AssetType Type = AssetType::None;
	};
} // namespace Imagine::Core

