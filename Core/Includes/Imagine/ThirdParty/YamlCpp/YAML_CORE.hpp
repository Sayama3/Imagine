//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include "YAML_DEFINE.hpp"

#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Core/UUID.hpp"

namespace YAML {
	YAML_SIMPLE_EMITTER_FUNC(std::filesystem::path, v.string());

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::Path &v) {
		out << KEYVAL("Source", Imagine::Core::GetFileSourceName(v.source));
		out << KEYVAL("Path", v.path);
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::UUID &v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (const auto &value: v) {
			out << value;
		}
		out << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::Buffer &v) {
		out << YAML::BeginSeq;
		{
			for (uint64_t i = 0; i < v.Size(); ++i) {
				out << v.Get<uint8_t>(i);
			}
		}
		out << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::ConstBufferView &v) {
		out << YAML::BeginSeq;
		{
			for (uint64_t i = 0; i < v.Size(); ++i) {
				out << v.At<uint8_t>(i);
			}
		}
		out << YAML::EndSeq;
		return out;
	}
} // namespace YAML

namespace YAML {
	template<>
	struct convert<::Imagine::Core::UUID> {
		inline static Node encode(const ::Imagine::Core::UUID &v) {
			Node node;
			for (const uint64_t &val: v) {
				node.push_back(val);
			}
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::Core::UUID &v) {
			if (!node.IsSequence() || node.size() != 2) return false;
			static_assert(sizeof(::Imagine::Core::UUID) == 32);
			const uint64_t low = node[0].as<uint64_t>(0);
			const uint64_t high = node[1].as<uint64_t>(0);
			v = ::Imagine::Core::UUID{low, high};
			return true;
		}
	};

	template<>
	struct convert<std::filesystem::path> {
		inline static Node encode(const std::filesystem::path &v) {
			Node node;
			node = v.string();
			return node;
		}

		inline static bool decode(const Node &node, std::filesystem::path &v) {
			v = node.as<std::string>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::Path> {
		inline static Node encode(const ::Imagine::Core::Path &v) {
			Node node;
			node["Source"] = Imagine::Core::GetFileSourceName(v.source);
			node["Path"] = v.path;
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::Core::Path &v) {
			if (!node.IsMap() || node.size() != 2) return false;
			const bool hasSource = Imagine::Core::TryGetFileSourceFromName(node["Source"].as<std::string>(), v.source);
			if (!hasSource) return false;
			v.path = node["Path"].as<std::filesystem::path>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::Buffer> {
		inline static Node encode(const ::Imagine::Core::Buffer &v) {
			Node node;
			for (uint64_t i = 0; i < v.Size(); ++i) {
				node.push_back(v.Get<uint8_t>(i));
			}
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::Core::Buffer &v) {
			if (node.IsNull() || !node.IsDefined()) {
				v.Release();
				return true;
			}

			if (!node.IsSequence()) {
				return false;
			}

			v.Reallocate(node.size());
			for (uint64_t i = 0; i < node.size(); ++i) {
				v.Get<uint8_t>(i) = node[i].as<uint8_t>();
			}
			return true;
		}
	};
} // namespace YAML
