//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Assets/AssetType.hpp"


namespace YAML {
	YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::AssetHandle &v);
	YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::AssetType &rhs);
} // namespace Imagine::ThirdParty::YamlCpp

namespace YAML {
	template<>
	struct convert<::Imagine::Core::AssetHandle>
	{
		inline static Node encode(const ::Imagine::Core::AssetHandle& rhs)
		{
			Node node;
			node.push_back(rhs.GetID());
			return node;
		}

		inline static bool decode(const Node& node, ::Imagine::Core::AssetHandle& rhs)
		{
			if(!node.IsSequence() || node.size() != 2) return false;
			Imagine::Core::UUID id = node.as<Imagine::Core::UUID>(Imagine::Core::UUID{0,0});
			rhs = Imagine::Core::AssetHandle{id};
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::AssetType>
	{
		inline static Node encode(const ::Imagine::Core::AssetType& rhs)
		{
			Node node;
			node.push_back(Imagine::Core::AssetTypeToString(rhs));
			return node;
		}

		inline static bool decode(const Node& node, ::Imagine::Core::AssetType& rhs)
		{
			auto str = node.as<std::string>(std::string());
			return ::Imagine::Core::TryAssetTypeFromString(str, rhs);
		}
	};
}