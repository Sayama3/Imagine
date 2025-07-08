//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>
#include "Imagine/Math/Core.hpp"
#include "Imagine/Math/Types.hpp"


namespace YAML {
	template<glm::length_t L, typename T, glm::qualifier Q>
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const glm::vec<L, T, Q> &v) {
		out << YAML::Flow;
		out << YAML::BeginSeq;
		for (int i = 0; i < L; ++i) {
			out << v[i];
		}
		out << YAML::EndSeq;
		return out;
	}

	template<typename T, glm::qualifier Q>
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const glm::qua<T, Q> &v) {
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
		return out;
	}

	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const glm::mat<C, R, T, Q> &v) {
		out << YAML::BeginSeq;
		for (int r = 0; r < R; ++r) {
			out << YAML::BeginSeq;
			out << YAML::Flow;
			for (int c = 0; c < C; ++c) {
				out << v[r][c];
			}
			out << YAML::EndSeq;
		}
		out << YAML::EndSeq;
		return out;
	}

	// TODO: Add Line, Ray, Plane, Circle, Sphere, Rect, BoundingBox, BoundingSphere

} // namespace Imagine::ThirdParty::YamlCpp
namespace YAML {
	template<glm::length_t L, typename T, glm::qualifier Q>
	struct convert<glm::vec<L, T, Q>> {
		inline static Node encode(const glm::vec<L, T, Q> &v) {
			Node node;
			for (int i = 0; i < L; ++i) {
				node.push_back(v[i]);
			}
			return node;
		}
		inline static bool decode(const Node &node, glm::vec<L, T, Q> &v) {
			if (!node.IsSequence() || node.size() != L) return false;
			for (int i = 0; i < L; ++i) {
				v[i] = node[i].as<T>();
			}
			return true;
		}
	};

	template<typename T, glm::qualifier Q>
	struct convert<glm::qua<T, Q>> {
		inline static Node encode(const glm::qua<T, Q> &v) {
			Node node;
			node.push_back(v.x);
			node.push_back(v.y);
			node.push_back(v.z);
			node.push_back(v.w);
			return node;
		}
		inline static bool decode(const Node &node, glm::qua<T, Q> &v) {
			if (!node.IsSequence() || node.size() != 4) return false;
			v.x = node[0].as<float>();
			v.y = node[1].as<float>();
			v.z = node[2].as<float>();
			v.w = node[3].as<float>();
			return true;
		}
	};

	template<glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
	struct convert<glm::mat<C, R, T, Q>> {
		inline static Node encode(const glm::mat<C, R, T, Q> &v) {
			Node node;
			for (int r = 0; r < R; ++r) {
				Node rowNode;
				for (int c = 0; c < C; ++c) {
					rowNode.push_back(v[r][c]);
				}
				node.push_back(rowNode);
			}
			return node;
		}
		inline static bool decode(const Node &node, glm::mat<C, R, T, Q> &v) {
			if (!node.IsSequence() || node.size() != R) return false;
			for (int r = 0; r < R; ++r) {
				Node rowNode = node[r];
				if (!rowNode.IsSequence() || rowNode.size() != C) return false;
				for (int c = 0; c < C; ++c) {
					v[r][c] = rowNode[c].as<T>();
				}
			}
			return true;
		}
	};
} // namespace YAML
