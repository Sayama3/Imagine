//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include <utility>

#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Assets/Asset.hpp"


namespace Imagine::Core
{
	struct AssetMetadata {
		inline AssetMetadata() = default;
		inline AssetMetadata(Path p, AssetType t) : FilePath(std::move(p)), Handle(), Type(t){}
		Path FilePath;
		AssetHandle Handle;
		AssetType Type = AssetType::None;

		inline operator bool() const {
			return Type != AssetType::None && Handle != NullAssetHandle;
		}
		inline bool operator ==(const AssetMetadata& other ) const {
			return Handle == other.Handle && Type == other.Type && FilePath == other.FilePath;
		}
		inline bool operator !=(const AssetMetadata& other) const {
			return !(*this == other);
		}
	};


} // namespace Imagine::Core

