//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>

namespace YAML {
	template <typename T, size_t Size>
	inline Emitter& operator<<(Emitter& emitter, const std::array<T, Size>& v) {
		return EmitSeq(emitter, v);
	}

	template <typename T>
	inline Emitter& operator<<(Emitter& emitter, const std::unordered_set<T>& v) {
		return EmitSeq(emitter, v);
	}

	template <typename K, typename V>
	inline Emitter& operator<<(Emitter& emitter, const std::unordered_map<K, V>& m) {
		emitter << BeginMap;
		for (const auto& v : m)
			emitter << Key << v.first << Value << v.second;
		emitter << EndMap;
		return emitter;
	}

}