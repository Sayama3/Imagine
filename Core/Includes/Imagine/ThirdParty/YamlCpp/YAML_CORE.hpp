//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include "../YAML_DEFINE.hpp"

#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Core/UUID.hpp"

namespace YAML {

	inline YAML::Emitter &operator<<(YAML::Emitter &out, const std::filesystem::path &v) {
		out << v.string();
		return out;
	}
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
} // namespace Imagine::ThirdParty::YamlCpp

namespace YAML
{
	template<>
	struct convert<::Imagine::Core::UUID>
	{
		inline static Node encode(const ::Imagine::Core::UUID& rhs)
		{
			Node node;
			for (const uint64_t& val : rhs) {
				node.push_back(val);
			}
			return node;
		}

		inline static bool decode(const Node& node, ::Imagine::Core::UUID& rhs)
		{
			if(!node.IsSequence() || node.size() != 2) return false;
			static_assert(sizeof(::Imagine::Core::UUID) == 32);
			const uint64_t low = node[0].as<uint64_t>(0);
			const uint64_t high = node[1].as<uint64_t>(0);
			rhs = ::Imagine::Core::UUID{low, high};
			return true;
		}
	};

	template<>
	struct convert<std::filesystem::path>
	{
		inline static Node encode(const std::filesystem::path& rhs)
		{
			Node node;
			node = rhs.string();
			return node;
		}

		inline static bool decode(const Node& node, std::filesystem::path& rhs)
		{
			rhs = node.as<std::string>();
			return true;
		}
	};

	template<>
	struct convert<::Imagine::Core::Path>
	{
		inline static Node encode(const ::Imagine::Core::Path& rhs)
		{
			Node node;
			node["Source"] = Imagine::Core::GetFileSourceName(rhs.source);
			node["Path"] = rhs.path;
			return node;
		}

		inline static bool decode(const Node& node, ::Imagine::Core::Path& rhs)
		{
			if (!node.IsMap() || node.size() != 2) return false;
			const bool hasSource = Imagine::Core::TryGetFileSourceFromName(node["Source"].as<std::string>(), rhs.source);
			if (!hasSource) return false;
			rhs.path = node["Path"].as<std::filesystem::path>();
			return true;
		}
	};
}