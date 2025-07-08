//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include "YAML_DEFINE.hpp"
#include "YAML_CORE.hpp"
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Assets/AssetType.hpp"


namespace YAML {
	YAML_SIMPLE_MGN_EMITTER_FUNC(AssetHandle, v.GetID());
	YAML_SIMPLE_MGN_ENUM_EMITTER(AssetType);
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

	YAML_SIMPLE_MGN_ENUM_CONVERTER(AssetType);
}