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

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Path &v) {
		out << KEYVAL("Source", Imagine::FileSourceToString(v.source));
		out << KEYVAL("Path", v.path);
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::UUID &v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (const auto &value: v) {
			out << value;
		}
		out << YAML::EndSeq;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Buffer &v) {
		out << YAML::BeginMap;
		{
			out <<KEYVAL("Size", v.Size());
			out <<KEYVAL("Data", YAML::BeginSeq);
			const auto count = v.Count<uint32_t>();
			for (uint64_t i = 0; i < count; ++i) {
				out << v.Get<uint32_t>(i);
			}
			if ((count * 4) != v.Size()) {
				const auto left = v.Size() - (count * 4);
				uint32_t data{0};
				memcpy(&data,&v.Get<uint8_t>(count * 4), left);
				out << data;
			}
			out << YAML::EndSeq;
		}
		out << YAML::EndMap;
		return out;
	}

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::ConstBufferView &v) {
		out << YAML::BeginMap;
		{
			out <<KEYVAL("Size", v.Size());
			out <<KEYVAL("Data", YAML::BeginSeq);
			const auto count = v.Count<uint32_t>();
			for (uint64_t i = 0; i < count; ++i) {
				out << v.At<uint32_t>(i);
			}
			if ((count * 4) != v.Size()) {
				const auto left = v.Size() - (count * 4);
				uint32_t data{0};
				memcpy(&data,&v.At<uint8_t>(count * 4), left);
				out << data;
			}
			out << YAML::EndSeq;
		}
		out << YAML::EndMap;
		return out;
	}
} // namespace YAML

namespace YAML {
	template<>
	struct convert<::Imagine::UUID> {
		inline static Node encode(const ::Imagine::UUID &v) {
			Node node;
			for (const uint64_t &val: v) {
				node.push_back(val);
			}
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::UUID &v) {
			if (!node.IsSequence() || node.size() != 2) return false;
			static_assert(sizeof(::Imagine::UUID) == 16);
			const uint64_t low = node[0].as<uint64_t>(0);
			const uint64_t high = node[1].as<uint64_t>(0);
			v = ::Imagine::UUID{low, high};
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
	struct convert<::Imagine::Path> {
		inline static Node encode(const ::Imagine::Path &v) {
			Node node;
			node["Source"] = Imagine::FileSourceToString(v.source);
			node["Path"] = v.path;
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::Path &v) {
			if (!node.IsMap() || node.size() != 2) return false;
			const bool hasSource = Imagine::TryFileSourceFromString(node["Source"].as<std::string>(), v.source);
			if (!hasSource) return false;
			v.path = node["Path"].as<std::filesystem::path>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Buffer> {
		inline static Node encode(const ::Imagine::Buffer &v) {
			Node node;
			node["Size"] = v.Size();
			auto dataNode = node["Data"];
			const auto count = v.Count<uint32_t>();
			for (uint64_t i = 0; i < count; ++i) {
				dataNode.push_back(v.Get<uint32_t>(i));
			}
			if ((count * 4) != v.Size()) {
				const auto left = v.Size() - (count * 4);
				uint32_t data{0};
				memcpy(&data,&v.Get<uint8_t>(count * 4), left);
				dataNode.push_back(data);
			}
			return node;
		}

		inline static bool decode(const Node &node, ::Imagine::Buffer &v) {
			if (!node || !node.IsMap()) return false;
			if (!node["Size"]) return false;

			v.Reallocate(node["Size"].as<uint64_t>(0));
			if (v.Size() == 0) return true;

			const auto data = node["Data"];

			if (!data || data.IsNull() || !data.IsDefined()) {
				v.Release();
				return false;
			}

			if (!data.IsSequence()) {
				return false;
			}

			const auto count = v.Count<uint32_t>();
			for (uint64_t i = 0; i < count; ++i) {
				v.Get<uint32_t>(i) = data[i].as<uint32_t>();
			}
			if ((count * 4) != v.Size()) {
				const auto left = v.Size() - (count * 4);
				uint32_t u32 = data[count].as<uint32_t>();
				memcpy(&v.Get<uint8_t>(count * 4), &u32, left);
			}
			return true;
		}
	};
} // namespace YAML
