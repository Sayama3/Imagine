//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Assets/AssetMetadata.hpp"
#include "Imagine/Assets/AssetType.hpp"
#include "YAML_CORE.hpp"
#include "YAML_DEFINE.hpp"


namespace YAML {
	YAML_SIMPLE_MGN_EMITTER_FUNC(AssetHandle, v.GetID());
	YAML_SIMPLE_MGN_ENUM_EMITTER(AssetType);
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::AssetMetadata &v) {
		out << YAML::BeginMap;
		{
			out << YAML::Key << "FilePath" << YAML::Value << v.FilePath;
			out << YAML::Key << "Handle" << YAML::Value << v.Handle;
			out << YAML::Key << "Type" << YAML::Value << v.Type;
		}
		out << YAML::EndMap;
		return out;
	}
} // namespace YAML

namespace YAML {
	template<>
	struct convert<::Imagine::AssetHandle> {
		inline static Node encode(const ::Imagine::AssetHandle &v) {
			Node node;
			node.push_back(v.GetID());
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::AssetHandle &v) {
			if (!node.IsSequence() || node.size() != 2) return false;
			Imagine::UUID id = node.as<Imagine::UUID>(Imagine::UUID{0, 0});
			v = Imagine::AssetHandle{id};
			return true;
		}
	};

	YAML_SIMPLE_MGN_ENUM_CONVERTER(AssetType);

	template<>
	struct convert<Imagine::AssetMetadata> {
		inline static Node encode(const Imagine::AssetMetadata &v) {
			Node node;
			YAML_NODE_OUT_VAL(FilePath);
			YAML_NODE_OUT_VAL(Handle);
			YAML_NODE_OUT_VAL(Type);
			return node;
		}
		inline static bool decode(const Node &node, Imagine::AssetMetadata &v) {
			YAML_NODE_IN_VAL(FilePath, Path);
			YAML_NODE_IN_VAL(Handle, AssetHandle);
			YAML_NODE_IN_VAL(Type, AssetType);
			return true;
		}
	};
} // namespace YAML
