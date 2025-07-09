//
// Created by ianpo on 09/04/2024.
//

#pragma once

#include "Imagine/Core/Profiling.hpp"
#include "Imagine/Core/SmartPointers.hpp"
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Project/Project.hpp"

namespace Imagine::Core
{
	class AssetManager
	{
	public:
		inline static Ref<Asset> GetAsset(AssetHandle handle)
		{
			MGN_PROFILE_FUNCTION();
			return Project::GetActive()->GetAssetManager()->GetAsset(handle);
		}

		template<typename T>
		inline static Ref<T> GetAssetAs(AssetHandle handle)
		{
			Ref<Asset> asset = GetAsset(handle);
			if(!asset) return nullptr;

			MGN_CORE_CHECK(T::GetStaticType() == asset->GetType(), "The asset {} is of type {} and not {}.", handle, asset->GetType(), T::GetStaticType());
			if(T::GetStaticType() == asset->GetType())
				return CastPtr<T>(asset);
			return nullptr;
		}

		inline static bool IsAssetHandleValid(AssetHandle handle)
		{
			MGN_PROFILE_FUNCTION();
			return Project::GetActive()->GetAssetManager()->IsAssetHandleValid(handle);
		}
	};

} // namespace Imagine::Core

